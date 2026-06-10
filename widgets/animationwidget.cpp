#include "animationwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPen>

#include <QtMath>

namespace {
constexpr auto kLimitLabel = "Limit";
constexpr auto kTargetLabel = "Target";
constexpr auto kLimitWarningText = "Limit reached, stop playback";
}

AnimationWidget::AnimationWidget(QWidget *parent)
    : QWidget(parent)
    , m_actualDisplacement(0.0)
    , m_targetDisplacement(0.0)
    , m_displacementUpperLimit(1.5)
    , m_limitExceeded(false)
{
    setMinimumSize(240, 320);
    setAutoFillBackground(true);
}

void AnimationWidget::setActualDisplacement(double displacement)
{
    if (qFuzzyCompare(1.0 + m_actualDisplacement, 1.0 + displacement)) {
        return;
    }

    m_actualDisplacement = displacement;
    updateLimitState();
    update();
}

void AnimationWidget::setTargetDisplacement(double displacement)
{
    if (qFuzzyCompare(1.0 + m_targetDisplacement, 1.0 + displacement)) {
        return;
    }

    m_targetDisplacement = displacement;
    update();
}

void AnimationWidget::setDisplacementUpperLimit(double upperLimit)
{
    if (qFuzzyCompare(1.0 + m_displacementUpperLimit, 1.0 + upperLimit)) {
        return;
    }

    m_displacementUpperLimit = upperLimit;
    updateLimitState();
    update();
}

bool AnimationWidget::isLimitExceeded() const
{
    return m_limitExceeded;
}

qreal AnimationWidget::displacementToWingY(const QRectF &drawingArea,
                                           qreal ropeTopY,
                                           qreal displacement) const
{
    const qreal minWingY = ropeTopY + 40.0;
    const qreal maxWingY = drawingArea.bottom() - 70.0;
    const qreal displacementPixels = displacement * 120.0;
    return qBound(minWingY, maxWingY - displacementPixels, maxWingY);
}

void AnimationWidget::updateLimitState()
{
    const bool limitExceeded = m_actualDisplacement >= m_displacementUpperLimit;
    if (m_limitExceeded == limitExceeded) {
        return;
    }

    m_limitExceeded = limitExceeded;
    if (m_limitExceeded) {
        emit stopRequested();
    }
}

void AnimationWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(248, 250, 252));

    const QRectF drawingArea = rect().adjusted(24, 24, -24, -24);
    const QPointF drumCenter(drawingArea.center().x(), drawingArea.top() + 50.0);
    const qreal drumRadius = qMin(drawingArea.width(), drawingArea.height()) * 0.08;
    const qreal ropeTopY = drumCenter.y() + drumRadius;
    const qreal wingWidth = drawingArea.width() * 0.58;
    const qreal wingHeight = 18.0;
    const qreal wingY = displacementToWingY(drawingArea, ropeTopY, static_cast<qreal>(m_actualDisplacement));
    const qreal targetWingY = displacementToWingY(drawingArea, ropeTopY, static_cast<qreal>(m_targetDisplacement));
    const qreal limitWingY = displacementToWingY(drawingArea, ropeTopY, static_cast<qreal>(m_displacementUpperLimit));
    const QRectF wingRect(drawingArea.center().x() - wingWidth / 2.0,
                          wingY,
                          wingWidth,
                          wingHeight);
    const QPointF ropeStart(drumCenter.x(), ropeTopY);
    const QPointF ropeEnd(drumCenter.x(), wingRect.top());

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_limitExceeded ? QColor(255, 235, 238) : QColor(222, 226, 230));
    painter.drawRoundedRect(QRectF(drawingArea.left() + 8.0,
                                   drawingArea.top() + 8.0,
                                   drawingArea.width() - 16.0,
                                   drawingArea.height() - 16.0),
                            16.0,
                            16.0);

    painter.setPen(QPen(QColor(230, 126, 34), 2.0, Qt::DashDotLine));
    painter.drawLine(QPointF(drawingArea.left() + 20.0, limitWingY + wingHeight / 2.0),
                     QPointF(drawingArea.right() - 20.0, limitWingY + wingHeight / 2.0));
    painter.setPen(QColor(211, 84, 0));
    painter.drawText(QPointF(drawingArea.left() + 24.0, limitWingY - 12.0), QString::fromLatin1(kLimitLabel));

    painter.setPen(QPen(QColor(52, 73, 94), 2.0));
    painter.setBrush(QColor(149, 165, 166));
    painter.drawEllipse(drumCenter, drumRadius, drumRadius);

    painter.setPen(QPen(QColor(231, 76, 60), 2.0, Qt::DashLine));
    painter.drawLine(QPointF(drawingArea.left() + 20.0, targetWingY + wingHeight / 2.0),
                     QPointF(drawingArea.right() - 20.0, targetWingY + wingHeight / 2.0));
    painter.setPen(QColor(192, 57, 43));
    painter.drawText(QPointF(drawingArea.left() + 24.0, targetWingY + 18.0), QString::fromLatin1(kTargetLabel));

    painter.setPen(QPen(QColor(52, 152, 219), 4.0, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(ropeStart, ropeEnd);

    painter.setPen(QPen(m_limitExceeded ? QColor(192, 57, 43) : QColor(39, 174, 96), 2.0));
    painter.setBrush(m_limitExceeded ? QColor(231, 76, 60) : QColor(46, 204, 113));
    painter.drawRoundedRect(wingRect, 6.0, 6.0);

    painter.setPen(QPen(m_limitExceeded ? QColor(192, 57, 43) : QColor(39, 174, 96), 2.0));
    painter.drawLine(QPointF(wingRect.left(), wingRect.center().y()),
                     QPointF(wingRect.right(), wingRect.center().y()));

    if (m_limitExceeded) {
        painter.setPen(QColor(192, 57, 43));
        painter.drawText(QRectF(drawingArea.left() + 16.0,
                                drawingArea.bottom() - 36.0,
                                drawingArea.width() - 32.0,
                                20.0),
                         Qt::AlignCenter,
                         QString::fromLatin1(kLimitWarningText));
    }
}
