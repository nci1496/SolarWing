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
    bool isLimitExceeded() const;

signals:
    void stopRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal displacementToWingY(const QRectF &drawingArea, qreal ropeTopY, qreal displacement) const;
    void updateLimitState();

    double m_actualDisplacement;
    double m_targetDisplacement;
    double m_displacementUpperLimit;
    bool m_limitExceeded;
};

#endif // ANIMATIONWIDGET_H
