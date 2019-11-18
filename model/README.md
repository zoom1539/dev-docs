# algo_config.json说明

配置文件目前固定字段有四项，其它的配置字段根据项目拟定，下列四个字段为固定项，务必在代码接口中实现

- `gpu_id`：设置程序在特定序号的GPU上运行（这个参数适应GPU算法及多GPU机器，默认为0，在第一块GPU上运行，为1则在第二块GPU上运行，以此类推）
- `draw_roi_area`：是否在结果图片或者视频中画出ROI感兴趣区域，`true`画，`false`不画
- `roi_color`：ROI框的颜色，RGB数组格式
- `show_result`：是否实时显示图片或者视频结果，`true`显示，`false`不显示
- `draw_result`：是否画出检测到的物体结果框 `true`画，`false`：不画
- `draw_confidence`：是否将置信度画在框顶部
- `thresh`：检测阈值，设置越大，召回率越高，设置越小，精确率越高
- `object_colors`：不同目标框的颜色，RGB数组格式
- `text_color`：目标框顶部文字的颜色
- `text_bg_color`：目标框顶部文字的背景颜色

样例配置文件
```json
{
    "gpu_id": 0,
    "draw_roi_area": true,
    "roi_color": [0, 255, 0],
    "draw_result": true,
    "draw_confidence": true,
    "thresh": 0.5,
    "object_colors": [
        {
            "dog": [0, 0, 255]
        }
    ],
    "text_color": [255, 255, 255],
    "text_bg_color": [120, 120, 120]
}

```
# model.data
模型数据