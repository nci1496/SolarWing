#include "plotwidget.h"

#include "../../QCustomPlot/qcustomplot.h"

#include <QEvent>
#include <QFont>
#include <QMargins>
#include <QMouseEvent>
#include <QPen>
#include <QPoint>
#include <QVBoxLayout>

namespace {

constexpr double kDefaultAxisMin = 0.0;
constexpr double kDefaultAxisMax = 1.0;

QColor defaultBackgroundColor()
{
    return QColor(245, 247, 250);
}

QColor defaultAxisColor()
{
    return QColor(90, 90, 90);
}

QColor defaultGridColor()
{
    return QColor(220, 224, 230);
}

QColor crosshairColor()
{
    return QColor(120, 120, 120, 180);
}

QColor markerOutlineColor()
{
    return QColor(44, 62, 80);
}

QColor markerFillColor()
{
    return QColor(255, 255, 255, 235);
}

} // namespace

PlotWidget::PlotWidget(const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_xLabel(QStringLiteral("时间 (s)"))
    , m_yLabel(QStringLiteral("数值"))
    , m_backgroundColor(defaultBackgroundColor())
    , m_textColor(defaultAxisColor())
    , m_displayFont(QStringLiteral("Microsoft YaHei"), 9)
    , m_plot(new QCustomPlot(this))
    , m_titleElement(nullptr)
    , m_verticalCrosshair(nullptr)
    , m_horizontalCrosshair(nullptr)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_plot);

    initializePlot();
    refreshPlot();
}

void PlotWidget::setTitle(const QString &title)
{
    m_title = title;
    if (m_titleElement) {
        m_titleElement->setText(m_title);
    }
    m_plot->replot();
}

void PlotWidget::setAxisLabels(const QString &xLabel, const QString &yLabel)
{
    m_xLabel = xLabel;
    m_yLabel = yLabel;
    m_plot->xAxis->setLabel(m_xLabel);
    m_plot->yAxis->setLabel(m_yLabel);
    m_plot->replot();
}

void PlotWidget::setSingleSeries(const QString &name,
                                 const QVector<double> &x,
                                 const QVector<double> &y,
                                 const QColor &color)
{
    SeriesData series;
    series.name = name;
    series.x = x;
    series.y = y;
    series.color = color;
    m_seriesList = {series};
    refreshPlot();
}

void PlotWidget::setSeriesList(const QVector<SeriesData> &seriesList)
{
    m_seriesList = seriesList;
    refreshPlot();
}

void PlotWidget::clearSeries()
{
    m_seriesList.clear();
    clearMarkers();
    refreshPlot();
}

void PlotWidget::setDisplayStyle(const QColor &backgroundColor,
                                 const QColor &textColor,
                                 const QFont &font)
{
    m_backgroundColor = backgroundColor;
    m_textColor = textColor;
    m_displayFont = font;
    applyBaseStyle();
    if (m_titleElement) {
        QFont titleFont = m_displayFont;
        titleFont.setPointSize(qMax(titleFont.pointSize() + 2, titleFont.pointSize()));
        titleFont.setBold(true);
        m_titleElement->setFont(titleFont);
        m_titleElement->setTextColor(m_textColor);
    }
    m_plot->replot();
}

bool PlotWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_plot) {
        return QWidget::eventFilter(watched, event);
    }

    switch (event->type()) {
    case QEvent::Enter:
        m_plot->setCursor(Qt::CrossCursor);
        return false;
    case QEvent::Leave:
        setCrosshairVisible(false);
        m_plot->unsetCursor();
        m_plot->replot(QCustomPlot::rpQueuedReplot);
        return false;
    case QEvent::MouseButtonPress: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_lastLeftPressPosition = mouseEvent->pos();
            m_dragging = false;
        }
        return false;
    }
    case QEvent::MouseMove: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if ((mouseEvent->buttons() & Qt::LeftButton) != 0
            && (mouseEvent->pos() - m_lastLeftPressPosition).manhattanLength() > 3) {
            m_dragging = true;
        }
        updateCrosshair(mouseEvent->pos());
        return false;
    }
    case QEvent::MouseButtonRelease: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && !m_dragging) {
            addMarkerAt(mouseEvent->pos());
        }
        if (mouseEvent->button() == Qt::LeftButton) {
            m_dragging = false;
        }
        return false;
    }
    case QEvent::MouseButtonDblClick: {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            clearMarkers();
            m_plot->replot(QCustomPlot::rpQueuedReplot);
        }
        return false;
    }
    default:
        return QWidget::eventFilter(watched, event);
    }
}

void PlotWidget::initializePlot()
{
    applyBaseStyle();

    m_plot->plotLayout()->insertRow(0);
    QFont titleFont = m_displayFont;
    titleFont.setPointSize(qMax(titleFont.pointSize() + 2, titleFont.pointSize()));
    titleFont.setBold(true);
    m_titleElement = new QCPTextElement(m_plot, m_title, titleFont);
    m_titleElement->setTextColor(m_textColor);
    m_plot->plotLayout()->addElement(0, 0, m_titleElement);

    m_plot->legend->setVisible(true);
    m_plot->legend->setBrush(QColor(255, 255, 255, 220));
    m_plot->legend->setBorderPen(QPen(QColor(210, 214, 220)));
    m_plot->legend->setFont(m_displayFont);

    m_plot->xAxis->setLabel(m_xLabel);
    m_plot->yAxis->setLabel(m_yLabel);
    m_plot->xAxis->setRange(kDefaultAxisMin, kDefaultAxisMax);
    m_plot->yAxis->setRange(kDefaultAxisMin, kDefaultAxisMax);

    m_verticalCrosshair = new QCPItemLine(m_plot);
    m_horizontalCrosshair = new QCPItemLine(m_plot);

    const QPen crosshairPen(crosshairColor(), 1, Qt::DashLine);
    m_verticalCrosshair->setPen(crosshairPen);
    m_horizontalCrosshair->setPen(crosshairPen);
    m_verticalCrosshair->setClipToAxisRect(true);
    m_horizontalCrosshair->setClipToAxisRect(true);
    m_verticalCrosshair->setClipAxisRect(m_plot->axisRect());
    m_horizontalCrosshair->setClipAxisRect(m_plot->axisRect());
    setCrosshairVisible(false);

    setupInteraction();
}

void PlotWidget::applyBaseStyle()
{
    m_plot->setBackground(m_backgroundColor);
    m_plot->plotLayout()->setMargins(QMargins(8, 8, 8, 8));

    const QPen axisPen(m_textColor);
    m_plot->xAxis->setBasePen(axisPen);
    m_plot->yAxis->setBasePen(axisPen);
    m_plot->xAxis->setTickPen(axisPen);
    m_plot->yAxis->setTickPen(axisPen);
    m_plot->xAxis->setSubTickPen(axisPen);
    m_plot->yAxis->setSubTickPen(axisPen);
    m_plot->xAxis->setTickLabelColor(m_textColor);
    m_plot->yAxis->setTickLabelColor(m_textColor);
    m_plot->xAxis->setLabelColor(m_textColor);
    m_plot->yAxis->setLabelColor(m_textColor);
    m_plot->xAxis->setTickLabelFont(m_displayFont);
    m_plot->yAxis->setTickLabelFont(m_displayFont);
    m_plot->xAxis->setLabelFont(m_displayFont);
    m_plot->yAxis->setLabelFont(m_displayFont);
    m_plot->legend->setFont(m_displayFont);
    m_plot->legend->setTextColor(m_textColor);

    m_plot->xAxis->grid()->setVisible(true);
    m_plot->yAxis->grid()->setVisible(true);
    m_plot->xAxis->grid()->setPen(QPen(defaultGridColor(), 1, Qt::DashLine));
    m_plot->yAxis->grid()->setPen(QPen(defaultGridColor(), 1, Qt::DashLine));
    m_plot->xAxis->grid()->setSubGridVisible(false);
    m_plot->yAxis->grid()->setSubGridVisible(false);
}

void PlotWidget::setupInteraction()
{
    m_plot->setMouseTracking(true);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    m_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    m_plot->installEventFilter(this);
}

void PlotWidget::refreshPlot()
{
    clearMarkers();
    m_plot->clearGraphs();

    for (const SeriesData &series : m_seriesList) {
        if (series.x.isEmpty() || series.y.isEmpty() || series.x.size() != series.y.size()) {
            continue;
        }

        QCPGraph *graph = m_plot->addGraph();
        graph->setName(series.name);
        graph->setData(series.x, series.y);
        graph->setPen(QPen(series.color, 2));
    }

    m_plot->legend->setVisible(!m_seriesList.isEmpty());
    updateAxisRanges();
    m_plot->replot();
}

void PlotWidget::updateAxisRanges()
{
    if (m_seriesList.isEmpty()) {
        m_plot->xAxis->setRange(kDefaultAxisMin, kDefaultAxisMax);
        m_plot->yAxis->setRange(kDefaultAxisMin, kDefaultAxisMax);
        return;
    }

    bool hasPoint = false;
    double xMin = 0.0;
    double xMax = 0.0;
    double yMin = 0.0;
    double yMax = 0.0;

    for (const SeriesData &series : m_seriesList) {
        const int count = qMin(series.x.size(), series.y.size());
        for (int i = 0; i < count; ++i) {
            const double xValue = series.x.at(i);
            const double yValue = series.y.at(i);
            if (!hasPoint) {
                xMin = xMax = xValue;
                yMin = yMax = yValue;
                hasPoint = true;
                continue;
            }
            xMin = qMin(xMin, xValue);
            xMax = qMax(xMax, xValue);
            yMin = qMin(yMin, yValue);
            yMax = qMax(yMax, yValue);
        }
    }

    if (!hasPoint) {
        m_plot->xAxis->setRange(kDefaultAxisMin, kDefaultAxisMax);
        m_plot->yAxis->setRange(kDefaultAxisMin, kDefaultAxisMax);
        return;
    }

    if (qFuzzyCompare(xMin, xMax)) {
        xMin -= 0.5;
        xMax += 0.5;
    }
    if (qFuzzyCompare(yMin, yMax)) {
        yMin -= 0.5;
        yMax += 0.5;
    }

    const double xPadding = qMax(0.1, (xMax - xMin) * 0.08);
    const double yPadding = qMax(0.1, (yMax - yMin) * 0.1);
    m_plot->xAxis->setRange(xMin - xPadding, xMax + xPadding);
    m_plot->yAxis->setRange(yMin - yPadding, yMax + yPadding);
}

void PlotWidget::updateCrosshair(const QPoint &position)
{
    if (!isInsideAxisRect(position)) {
        setCrosshairVisible(false);
        m_plot->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    const double xValue = m_plot->xAxis->pixelToCoord(position.x());
    const double yValue = m_plot->yAxis->pixelToCoord(position.y());

    m_verticalCrosshair->start->setCoords(xValue, m_plot->yAxis->range().lower);
    m_verticalCrosshair->end->setCoords(xValue, m_plot->yAxis->range().upper);
    m_horizontalCrosshair->start->setCoords(m_plot->xAxis->range().lower, yValue);
    m_horizontalCrosshair->end->setCoords(m_plot->xAxis->range().upper, yValue);
    setCrosshairVisible(true);
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void PlotWidget::setCrosshairVisible(bool visible)
{
    if (m_verticalCrosshair) {
        m_verticalCrosshair->setVisible(visible);
    }
    if (m_horizontalCrosshair) {
        m_horizontalCrosshair->setVisible(visible);
    }
}

void PlotWidget::addMarkerAt(const QPoint &position)
{
    if (!isInsideAxisRect(position)) {
        return;
    }

    QCPGraph *graph = pickReferenceGraph();
    if (!graph || graph->dataCount() == 0) {
        return;
    }

    const double clickedX = m_plot->xAxis->pixelToCoord(position.x());
    QSharedPointer<QCPGraphDataContainer> data = graph->data();
    QCPGraphDataContainer::const_iterator nearest = data->findBegin(clickedX);

    if (nearest == data->constEnd()) {
        nearest = data->constEnd() - 1;
    } else if (nearest != data->constBegin()) {
        QCPGraphDataContainer::const_iterator previous = nearest - 1;
        if (qAbs(previous->key - clickedX) < qAbs(nearest->key - clickedX)) {
            nearest = previous;
        }
    }

    MarkerItems marker;
    marker.tracer = new QCPItemTracer(m_plot);
    marker.tracer->setGraph(graph);
    marker.tracer->setGraphKey(nearest->key);
    marker.tracer->setInterpolating(false);
    marker.tracer->setStyle(QCPItemTracer::tsCircle);
    marker.tracer->setPen(QPen(markerOutlineColor(), 2));
    marker.tracer->setBrush(markerFillColor());
    marker.tracer->setSize(8);

    marker.label = new QCPItemText(m_plot);
    marker.label->position->setParentAnchor(marker.tracer->position);
    marker.label->position->setCoords(10, -10);
    marker.label->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    marker.label->setText(markerText(nearest->key, nearest->value));
    marker.label->setFont(QFont(QStringLiteral("Microsoft YaHei"), 8));
    marker.label->setPen(QPen(markerOutlineColor()));
    marker.label->setBrush(markerFillColor());
    marker.label->setPadding(QMargins(4, 2, 4, 2));

    m_markers.append(marker);
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void PlotWidget::clearMarkers()
{
    for (const MarkerItems &marker : m_markers) {
        if (marker.label) {
            m_plot->removeItem(marker.label);
        }
        if (marker.tracer) {
            m_plot->removeItem(marker.tracer);
        }
    }
    m_markers.clear();
}

bool PlotWidget::isInsideAxisRect(const QPoint &position) const
{
    return m_plot->axisRect()->rect().contains(position);
}

QCPGraph *PlotWidget::pickReferenceGraph() const
{
    return m_plot->graphCount() > 0 ? m_plot->graph(0) : nullptr;
}

QString PlotWidget::markerText(double xValue, double yValue) const
{
    return QStringLiteral("(%1, %2)")
        .arg(xValue, 0, 'f', 3)
        .arg(yValue, 0, 'f', 3);
}
