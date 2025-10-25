#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include "bubbleframe.h"

/******************************************************************************
 *
 * @file       picturebubble.h
 * @brief      Function: 图片气泡框
 *
 * @author     DustWind丶
 * @date       2025/05/11
 * @history
 *****************************************************************************/
class PictureBubble : public BubbleFrame
{
public:
    PictureBubble(ChatRole role, const QPixmap &picture, QWidget *parent = nullptr);
    ~PictureBubble();
};

#endif // PICTUREBUBBLE_H
