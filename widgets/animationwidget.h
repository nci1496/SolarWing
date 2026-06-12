#ifndef ANIMATIONWIDGET_H
#define ANIMATIONWIDGET_H

#include <QWidget>

class AnimationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnimationWidget(QWidget *parent = nullptr);

    void setActualDisplacement(double displacement);
    void setTargetDisplacement(double displacement);
    void setDisplacementUpperLimit(double upperLimit);
    void setErrorValue(double errorValue);
    void setPlaybackTime(double currentTime, double totalTime);
    void setPlaybackProgress(double progress);
    bool isLimitExceeded() const;

signals:
    void stopRequested();
    void progressSeekRequested(double progress);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    qreal displacementToWingY(qreal minWingY, qreal maxWingY, qreal displacement) const;
    void updateLimitState();
    QRectF progressBarRect() const;
    double progressFromPosition(qreal x) const;
    void updateProgressFromPosition(qreal x, bool emitSignal);

    double m_actualDisplacement;
    double m_targetDisplacement;
    double m_displacementUpperLimit;
    double m_errorValue;
    double m_currentTime;
    double m_totalTime;
    double m_playbackProgress;
    bool m_limitExceeded;
    bool m_draggingProgress;
};

#endif // ANIMATIONWIDGET_H
