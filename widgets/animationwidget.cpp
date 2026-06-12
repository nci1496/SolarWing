#include "animationwidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>

#include <QtMath>

namespace {
constexpr auto kLimitWarningText = "Limit reached, stop playback";
constexpr int kScaleTickCount = 5;
}

AnimationWidget::AnimationWidget(QWidget *parent)
    : QWidget(parent)
    , m_actualDisplacement(0.0)
    , m_targetDisplacement(0.0)
    , m_displacementUpperLimit(1.5)
    , m_errorValue(0.0)
    , m_currentTime(0.0)
    , m_totalTime(0.0)
    , m_playbackProgress(0.0)
    , m_limitExceeded(false)
    , m_draggingProgress(false)
{
    setMinimumSize(260, 380);
    setAutoFillBackground(true);
    setMouseTracking(true);
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
    const double safeUpperLimit = qMax(0.1, upperLimit);
    if (qFuzzyCompare(1.0 + m_displacementUpperLimit, 1.0 + safeUpperLimit)) {
        return;
    }

    m_displacementUpperLimit = safeUpperLimit;
    updateLimitState();
    update();
}

void AnimationWidget::setErrorValue(double errorValue)
{
    if (qFuzzyCompare(1.0 + m_errorValue, 1.0 + errorValue)) {
        return;
    }

    m_errorValue = errorValue;
    update();
}

void AnimationWidget::setPlaybackTime(double currentTime, double totalTime)
{
    const double safeCurrentTime = qMax(0.0, currentTime);
    const double safeTotalTime = qMax(0.0, totalTime);
    if (qFuzzyCompare(1.0 + m_currentTime, 1.0 + safeCurrentTime)
        && qFuzzyCompare(1.0 + m_totalTime, 1.0 + safeTotalTime)) {
        return;
    }

    m_currentTime = safeCurrentTime;
    m_totalTime = safeTotalTime;
    update();
}

void AnimationWidget::setPlaybackProgress(double progress)
{
    const double boundedProgress = qBound(0.0, progress, 1.0);
    if (qFuzzyCompare(1.0 + m_playbackProgress, 1.0 + boundedProgress)) {
        return;
    }

    m_playbackProgress = boundedProgress;
    update();
}

bool AnimationWidget::isLimitExceeded() const
{
    return m_limitExceeded;
}

qreal AnimationWidget::displacementToWingY(qreal minWingY, qreal maxWingY, qreal displacement) const
{
    const qreal safeUpperLimit = qMax<qreal>(0.1, static_cast<qreal>(m_displacementUpperLimit));
    const qreal boundedDisplacement = qBound<qreal>(0.0, displacement, safeUpperLimit);
    const qreal normalized = boundedDisplacement / safeUpperLimit;
    return maxWingY - normalized * (maxWingY - minWingY);
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

QRectF AnimationWidget::progressBarRect() const
{
    return QRectF(28.0, height() - 34.0, qMax(120, width() - 56), 12.0);
}

double AnimationWidget::progressFromPosition(qreal x) const
{
    const QRectF barRect = progressBarRect();
    if (barRect.width() <= 0.0) {
        return 0.0;
    }

    return qBound(0.0, (x - barRect.left()) / barRect.width(), 1.0);
}

void AnimationWidget::updateProgressFromPosition(qreal x, bool emitSignal)
{
    const double progress = progressFromPosition(x);
    setPlaybackProgress(progress);
    if (emitSignal) {
        emit progressSeekRequested(progress);
    }
}

void AnimationWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && progressBarRect().adjusted(-6.0, -6.0, 6.0, 6.0).contains(event->position())) {
        m_draggingProgress = true;
        updateProgressFromPosition(event->position().x(), true);
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void AnimationWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingProgress) {
        updateProgressFromPosition(event->position().x(), true);
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void AnimationWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_draggingProgress && event->button() == Qt::LeftButton) {
        m_draggingProgress = false;
        updateProgressFromPosition(event->position().x(), true);
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void AnimationWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(248, 250, 252));

    const QRectF outerPanel = rect().adjusted(16, 16, -16, -52);
    const QRectF infoRect(outerPanel.left() + 10.0, outerPanel.top() + 10.0, outerPanel.width() - 20.0, 56.0);
    const QRectF drawingArea(outerPanel.left() + 10.0, infoRect.bottom() + 10.0, outerPanel.width() - 20.0, outerPanel.height() - infoRect.height() - 28.0);
    const QRectF scaleRect(drawingArea.right() - 34.0, drawingArea.top() + 10.0, 28.0, drawingArea.height() - 20.0);
    const QRectF animationRect(drawingArea.left() + 10.0, drawingArea.top() + 10.0, drawingArea.width() - 54.0, drawingArea.height() - 20.0);

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_limitExceeded ? QColor(255, 235, 238) : QColor(222, 226, 230));
    painter.drawRoundedRect(outerPanel, 16.0, 16.0);

    painter.setBrush(QColor(255, 255, 255, 220));
    painter.drawRoundedRect(infoRect, 10.0, 10.0);

    painter.setPen(QColor(44, 62, 80));
    QFont infoFont = painter.font();
    infoFont.setPointSize(10);
    painter.setFont(infoFont);
    const QString targetText = QStringLiteral("Target=%1").arg(m_targetDisplacement, 0, 'f', 2);
    const QString actualText = QStringLiteral("Actual=%1").arg(m_actualDisplacement, 0, 'f', 2);
    const QString limitText = QStringLiteral("Limit=%1").arg(m_displacementUpperLimit, 0, 'f', 2);
    const QString errorText = QStringLiteral("Error=%1").arg(m_errorValue, 0, 'f', 2);
    painter.drawText(QRectF(infoRect.left() + 12.0, infoRect.top() + 8.0, infoRect.width() / 2.0 - 8.0, 20.0), Qt::AlignLeft | Qt::AlignVCenter, targetText);
    painter.drawText(QRectF(infoRect.left() + infoRect.width() / 2.0, infoRect.top() + 8.0, infoRect.width() / 2.0 - 12.0, 20.0), Qt::AlignLeft | Qt::AlignVCenter, actualText);
    painter.drawText(QRectF(infoRect.left() + 12.0, infoRect.top() + 30.0, infoRect.width() / 2.0 - 8.0, 20.0), Qt::AlignLeft | Qt::AlignVCenter, limitText);
    painter.drawText(QRectF(infoRect.left() + infoRect.width() / 2.0, infoRect.top() + 30.0, infoRect.width() / 2.0 - 12.0, 20.0), Qt::AlignLeft | Qt::AlignVCenter, errorText);

    const QPointF drumCenter(animationRect.center().x(), animationRect.top() + 34.0);
    const qreal drumRadius = qMin(animationRect.width(), animationRect.height()) * 0.08;
    const qreal ropeTopY = drumCenter.y() + drumRadius;
    const qreal minWingY = ropeTopY + 34.0;
    const qreal maxWingY = animationRect.bottom() - 26.0;
    const qreal wingWidth = animationRect.width() * 0.58;
    const qreal wingHeight = 18.0;
    const qreal wingY = displacementToWingY(minWingY, maxWingY, static_cast<qreal>(m_actualDisplacement));
    const qreal targetWingY = displacementToWingY(minWingY, maxWingY, static_cast<qreal>(m_targetDisplacement));
    const qreal limitWingY = displacementToWingY(minWingY, maxWingY, static_cast<qreal>(m_displacementUpperLimit));
    const QRectF wingRect(animationRect.center().x() - wingWidth / 2.0, wingY, wingWidth, wingHeight);
    const QPointF ropeStart(drumCenter.x(), ropeTopY);
    const QPointF ropeEnd(drumCenter.x(), wingRect.top());

    painter.setPen(QPen(QColor(52, 73, 94), 2.0));
    painter.setBrush(QColor(149, 165, 166));
    painter.drawEllipse(drumCenter, drumRadius, drumRadius);

    painter.setPen(QPen(QColor(230, 126, 34), 2.0, Qt::DashDotLine));
    painter.drawLine(QPointF(animationRect.left() + 6.0, limitWingY + wingHeight / 2.0), QPointF(animationRect.right() - 10.0, limitWingY + wingHeight / 2.0));
    painter.setPen(QColor(211, 84, 0));
    painter.drawText(QRectF(animationRect.left() + 8.0, limitWingY - 24.0, 80.0, 18.0), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Limit"));

    painter.setPen(QPen(QColor(231, 76, 60), 2.0, Qt::DashLine));
    painter.drawLine(QPointF(animationRect.left() + 6.0, targetWingY + wingHeight / 2.0), QPointF(animationRect.right() - 10.0, targetWingY + wingHeight / 2.0));
    painter.setPen(QColor(192, 57, 43));
    painter.drawText(QRectF(animationRect.left() + 8.0, targetWingY + 6.0, 80.0, 18.0), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Target"));

    painter.setPen(QPen(QColor(52, 152, 219), 4.0, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(ropeStart, ropeEnd);

    painter.setPen(QPen(m_limitExceeded ? QColor(192, 57, 43) : QColor(39, 174, 96), 2.0));
    painter.setBrush(m_limitExceeded ? QColor(231, 76, 60) : QColor(46, 204, 113));
    painter.drawRoundedRect(wingRect, 6.0, 6.0);
    painter.drawLine(QPointF(wingRect.left(), wingRect.center().y()), QPointF(wingRect.right(), wingRect.center().y()));

    painter.setPen(QPen(QColor(127, 140, 141), 1.2));
    painter.drawLine(QPointF(scaleRect.center().x(), minWingY), QPointF(scaleRect.center().x(), maxWingY));
    for (int tickIndex = 0; tickIndex <= kScaleTickCount; ++tickIndex) {
        const qreal ratio = static_cast<qreal>(tickIndex) / kScaleTickCount;
        const qreal tickY = maxWingY - ratio * (maxWingY - minWingY);
        const double tickValue = ratio * m_displacementUpperLimit;
        painter.drawLine(QPointF(scaleRect.center().x() - 6.0, tickY), QPointF(scaleRect.center().x() + 4.0, tickY));
        painter.drawText(QRectF(scaleRect.left() - 28.0, tickY - 10.0, 24.0, 20.0), Qt::AlignRight | Qt::AlignVCenter, QString::number(tickValue, 'f', 1));
    }

    painter.setPen(m_limitExceeded ? QColor(192, 57, 43) : QColor(44, 62, 80));
    painter.drawText(QRectF(outerPanel.left(), outerPanel.bottom() - 22.0, outerPanel.width(), 18.0), Qt::AlignCenter, QStringLiteral("t=%1 / %2").arg(m_currentTime, 0, 'f', 2).arg(m_totalTime, 0, 'f', 2));

    if (m_limitExceeded) {
        painter.setPen(QColor(192, 57, 43));
        painter.drawText(QRectF(animationRect.left(), animationRect.bottom() - 10.0, animationRect.width(), 18.0), Qt::AlignCenter, QString::fromLatin1(kLimitWarningText));
    }

    const QRectF barRect = progressBarRect();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(210, 218, 226));
    painter.drawRoundedRect(barRect, 6.0, 6.0);

    const QRectF filledRect(barRect.left(), barRect.top(), barRect.width() * m_playbackProgress, barRect.height());
    painter.setBrush(QColor(52, 152, 219));
    painter.drawRoundedRect(filledRect, 6.0, 6.0);

    const qreal knobX = barRect.left() + barRect.width() * m_playbackProgress;
    painter.setBrush(QColor(41, 128, 185));
    painter.drawEllipse(QPointF(knobX, barRect.center().y()), 7.0, 7.0);
}
