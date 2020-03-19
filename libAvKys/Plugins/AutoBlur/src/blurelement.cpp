/* Webcamoid, webcam capture application.
 * Copyright (C) 2016  Gonzalo Exequiel Pedone
 *
 * Webcamoid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Webcamoid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

#include <QImage>
#include <QQmlContext>
#include <akpacket.h>
#include <akvideopacket.h>
#include <opencv2/video.hpp>
#include <iostream>

#include "blurelement.h"

class BlurElementPrivate
{
    public:
        int m_radius {5};
        cv::Ptr<cv::BackgroundSubtractor> m_backSub;
        cv::Mat m_fgMask;
        int m_wantedFrames = 0;
        bool m_maskReset = false;

        BlurElementPrivate() {
            this->m_backSub = cv::createBackgroundSubtractorMOG2();
        }
};

BlurElement::BlurElement():
    AkElement()
{
    this->d = new BlurElementPrivate;
}

BlurElement::~BlurElement()
{
    delete this->d;
}

int BlurElement::radius() const
{
    return this->d->m_radius;
}

void BlurElement::handleCaptureBaseline() const
{
    this->d->m_wantedFrames = 30;
    this->d->m_maskReset = true;
}

QString BlurElement::controlInterfaceProvide(const QString &controlId) const
{
    Q_UNUSED(controlId)

    return QString("qrc:/AutoBlur/share/qml/main.qml");
}

void BlurElement::controlInterfaceConfigure(QQmlContext *context,
                                            const QString &controlId) const
{
    Q_UNUSED(controlId)

    context->setContextProperty("AutoBlur", const_cast<QObject *>(qobject_cast<const QObject *>(this)));
    context->setContextProperty("controlId", this->objectName());
}

AkPacket BlurElement::iVideoStream(const AkVideoPacket &packet)
{
    auto src = packet.toImage();

    if (src.isNull())
        return AkPacket();

    src = src.convertToFormat(QImage::Format_RGB888);
    QImage buf = src.copy();

    cv::Mat cvFrame(buf.height(), buf.width(), CV_8UC3, buf.bits(), buf.bytesPerLine());
    cv::cvtColor(cvFrame, cvFrame, cv::COLOR_RGB2BGR);

    cv::Mat bgFrame;
    cv::GaussianBlur(cvFrame, bgFrame, cv::Size2i(0, 0), this->d->m_radius);

    double learningRate = 0;
    if (this->d->m_wantedFrames > 0) {
        learningRate = this->d->m_maskReset ? 1 : -1;
        this->d->m_wantedFrames--;
    }
    this->d->m_backSub->apply(
        bgFrame,
        this->d->m_fgMask,
        learningRate
    );
    this->d->m_maskReset = false;

    cv::Mat fgMask;
    cv::GaussianBlur(this->d->m_fgMask, fgMask, cv::Size2i(0, 0), this->d->m_radius);
    cv::threshold(fgMask, fgMask, 200, 255, cv::THRESH_TOZERO);

    cv::Mat outFrame;
    cv::GaussianBlur(cvFrame, outFrame, cv::Size2i(0, 0), this->d->m_radius);
    cvFrame.copyTo(outFrame, fgMask);

    if (this->d->m_wantedFrames > 0) {
        cv::putText(
                    outFrame,
                    "Establishing baseline...",
                    cv::Point(100, 100),
                    cv::FONT_HERSHEY_SIMPLEX, 1,
                    cv::Scalar(255, 255, 255)
        );
    }

    cv::cvtColor(outFrame, outFrame, cv::COLOR_BGR2RGB);
    QImage oFrame(outFrame.data, outFrame.cols, outFrame.rows, outFrame.step, QImage::Format_RGB888);
    auto oPacket = AkVideoPacket::fromImage(oFrame, packet);
    akSend(oPacket)
}

void BlurElement::setRadius(int radius)
{
    if (this->d->m_radius == radius)
        return;

    this->d->m_radius = radius;
    emit this->radiusChanged(radius);
}

void BlurElement::resetRadius()
{
    this->setRadius(5);
}

#include "moc_blurelement.cpp"
