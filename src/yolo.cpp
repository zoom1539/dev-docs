#include "yolo.h"

bool yolo::init_table()
{
	for (int i = 0; i < 256; i++)
	{
		m_table[i] = i * 1.0 / 255.0f;
	}
	return true;
}

yolo::yolo(const char *cfgfile, const char *weightfile, const char* labelfile, float thresh, int gpu_id)
{
    //list *options = read_data_cfg((char*)"darknet/coco.data");
    list *options = read_data_cfg((char*)labelfile);
    char *name_list = option_find_str(options, (char*)"names", (char*)"");
    //char *name_list = option_find_str(options, (char*)"names", (char*)"data/names.list");
    printf("name_list:%s",name_list);
    m_label_names = get_labels(name_list);
    cuda_set_device(gpu_id);

	m_net = load_network((char*)cfgfile, (char*)weightfile, 0);
	set_batch_network(m_net, 1);
	m_thresh = thresh;

	//初始化转换表
	this->init_table();
}

yolo::~yolo()
{

}

image yolo::mat_to_img(cv::Mat& mat) {
    int h = mat.rows;
    int w = mat.cols;
    int wh = w*h;
    image out = make_image(w, h, 3);
    unsigned char* mat_data = mat.data;
    for(int c=0;c<3;c++){
        for(int i=0;i<w;++i){
            for(int j=0;j<h;++j){
                out.data[c*wh+j*w+i]= m_table[mat.at<cv::Vec3b>(j,i)[c]];
                //out.data[c*wh+j*w+i]= mat.at<cv::Vec3b>(j,i)[c]/255.;
            }
        }
    }
    rgbgr_image(out);
    return out;
}

int yolo::detect(cv::Mat& mat, VEC_TARGET& vecTarget)
{
    //double starttime=what_time_is_it_now();
    image im_temp = mat_to_img(mat);
    image im = letterbox_image(im_temp, m_net->w, m_net->h);
    layer l = m_net->layers[m_net->n-1];

    float *X = im.data;

    double time=what_time_is_it_now();
    network_predict(m_net, X);
//    printf("Predicted in %f seconds.\n", what_time_is_it_now()-time);

    int nboxes = 0;
    detection *dets = get_network_boxes(m_net, mat.cols, mat.rows, 0.5, 0.5, 0, 1, &nboxes);

    float nms=0.45;
    do_nms_sort(dets, nboxes, l.classes, nms);

    for(int i = 0; i < nboxes; ++i){
        char labelstr[4096] = {0};
        int class_type = -1;
        int j = 0;
        for(; j < l.classes; ++j){
            if (dets[i].prob[j] > m_thresh){
                if (class_type < 0) {
                    strcat(labelstr, m_label_names[j]);
                    class_type = j;
                } else {
                    strcat(labelstr, ", ");
                    strcat(labelstr, m_label_names[j]);
                }
//                printf("%s(%d): %.0f%%\n", m_label_names[j], j, dets[i].prob[j]*100);

                //只过滤出人的标签
                // if (class_type != 0)
                // 	continue;
                

                //填充vecTarget
            	CvTarget target;
                target.class_type = class_type;

            	target.confidence = dets[i].prob[j];

                int left  = (dets[i].bbox.x-dets[i].bbox.w/2.)*mat.cols;
                int right = (dets[i].bbox.x+dets[i].bbox.w/2.)*mat.cols;
                int top   = (dets[i].bbox.y-dets[i].bbox.h/2.)*mat.rows;
                int bot   = (dets[i].bbox.y+dets[i].bbox.h/2.)*mat.rows;

                if(left < 0) left = 0;
                if(right > mat.cols-1) right = mat.cols-1;
                if(top < 0) top = 0;
                if(bot > mat.rows-1) bot = mat.rows-1;

            	target.rect.x = left;
            	target.rect.y = top;
            	target.rect.width = right - left;
            	target.rect.height = bot - top;

            	target.center.x = target.rect.x + target.rect.width / 2;
            	target.center.y = target.rect.y + target.rect.height / 2;

//            	IF_NOT(target.rect.x && target.rect.y && target.rect.width && target.rect.height)
//            	{
//            		continue;
//            	}

            	target.target_img = mat(target.rect).clone();

            	vecTarget.push_back(target);
            }
        }
    }

    free_detections(dets, nboxes);
    free_image(im_temp);
    free_image(im);

//    printf("all in %f seconds.\n", what_time_is_it_now()-starttime);

    return vecTarget.size();
}

