#ifndef WIDGETS_PLOTWIDGET_H
#define WIDGETS_PLOTWIDGET_H

#include <QPoint>
#include <QVector>
#include <QWidget>

class QColor;
class QFont;

class QCPGraph;
class QCPItemLine;
class QCPItemText;
class QCPItemTracer;
class QCPTextElement;
class QCustomPlot;
class QMouseEvent;

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    struct SeriesData {
        QString name;
        QVector<double> x;
        QVector<double> y;
        QColor color;
    };

    explicit PlotWidget(const QString &title = QString(), QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setAxisLabels(const QString &xLabel, const QString &yLabel);
    void setSingleSeries(const QString &name,
                         const QVector<double> &x,
                         const QVector<double> &y,
                         const QColor &color = QColor(64, 114, 203));
    void setSeriesList(const QVector<SeriesData> &seriesList);
    void clearSeries();
    void setDisplayStyle(const QColor &backgroundColor,
                         const QColor &textColor,
                         const QFont &font);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    struct MarkerItems {
        QCPItemTracer *tracer = nullptr;
        QCPItemText *label = nullptr;
    };

    void initializePlot();
    void applyBaseStyle();
    void setupInteraction();
    void refreshPlot();
    void updateAxisRanges();
    void updateCrosshair(const QPoint &position);
    void setCrosshairVisible(bool visible);
    void addMarkerAt(const QPoint &position);
    void clearMarkers();
    bool isInsideAxisRect(const QPoint &position) const;
    QCPGraph *pickReferenceGraph() const;
    QString markerText(double xValue, double yValue) const;

    QString m_title;
    QString m_xLabel;
    QString m_yLabel;
    QVector<SeriesData> m_seriesList;
    QVector<MarkerItems> m_markers;
    QPoint m_lastLeftPressPosition;
    bool m_dragging = false;
    QColor m_backgroundColor;
    QColor m_textColor;
    QFont m_displayFont;
    QCustomPlot *m_plot;
    QCPTextElement *m_titleElement;
    QCPItemLine *m_verticalCrosshair;
    QCPItemLine *m_horizontalCrosshair;
};

#endif // WIDGETS_PLOTWIDGET_H
