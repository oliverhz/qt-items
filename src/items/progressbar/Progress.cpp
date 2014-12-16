#include "Progress.h"
#include <QStyleOptionProgressBar>

namespace Qi
{

ViewProgressBase::ViewProgressBase(const QSharedPointer<ModelProgress>& model)
    : ViewModeled<ModelProgress>(model)
{
}

ViewProgressContents::ViewProgressContents(const QSharedPointer<ModelProgress>& model)
    : ViewProgressBase(model)
{
}

void ViewProgressContents::drawImpl(QPainter* painter, const GuiContext& ctx, const CacheContext& cache, bool* /*showTooltip*/) const
{
    float value = theModel()->value(cache.item);
    if (tuneValue)
    {
        if (!tuneValue(value, cache.item))
            return;
    }

    Q_ASSERT(value >= 0.f || qFuzzyCompare(value, 0.f));
    Q_ASSERT(value <= 1.f || qFuzzyCompare(value, 1.f));
    value = qBound(0.f, value, 1.f);

    auto style = ctx.style();

    QStyleOptionProgressBar option;
    ctx.initStyleOption(option);
    option.textVisible = false;
    option.minimum = 0;
    option.maximum = 100;
    option.progress = int(100.f * value);

    // dont initialize styleObject from widget for QWindowsVistaStyle
    // this disables buggous animations
    if (style->inherits("QWindowsVistaStyle"))
        option.styleObject = nullptr;

    option.rect = cache.cacheView.rect();

    // draw progress
    style->drawControl(QStyle::CE_ProgressBarContents, &option, painter, ctx.widget);
}

ViewProgressLabel::ViewProgressLabel(const QSharedPointer<ModelProgress>& model, ProgressLabelMode mode)
    : ViewProgressBase(model),
      m_mode(mode)
{
}

void ViewProgressLabel::drawImpl(QPainter* painter, const GuiContext& ctx, const CacheContext& cache, bool* /*showTooltip*/) const
{
    float value = theModel()->value(cache.item);
    if (tuneValue)
    {
        if (!tuneValue(value, cache.item))
            return;
    }

    Q_ASSERT(value >= 0.f || qFuzzyCompare(value, 0.f));
    Q_ASSERT(value <= 1.f || qFuzzyCompare(value, 1.f));
    value = qBound(0.f, value, 1.f);

    auto style = ctx.style();

    QStyleOptionProgressBar option;
    ctx.initStyleOption(option);
    option.textVisible = true;
    option.minimum = 0;
    option.maximum = 100;
    option.progress = int(100.f * value);

    switch (m_mode)
    {
    case ProgressLabelModeNormalized:
        option.text.setNum(value, 'g', 2);
        break;

    case ProgressLabelModeProgress:
        option.text.setNum(value*100.f, 'g', 2);
        break;

    case ProgressLabelModePercent:
        option.text.setNum(value*100.f, 'g', 2);
        option.text += '%';
        break;
    }

    // dont initialize styleObject from widget for QWindowsVistaStyle
    // this disables buggous animations
    if (style->inherits("QWindowsVistaStyle"))
        option.styleObject = nullptr;

    option.rect = cache.cacheView.rect();

    // draw progress
    style->drawControl(QStyle::CE_ProgressBarLabel, &option, painter, ctx.widget);
}

ViewProgressBox::ViewProgressBox(const QSharedPointer<ModelProgress>& model)
    : ViewProgressBase(model)
{
    contentsColor = Qt::magenta;
    boundsColor = contentsColor.darker();
}

void ViewProgressBox::drawImpl(QPainter* painter, const GuiContext& /*ctx*/, const CacheContext& cache, bool* /*showTooltip*/) const
{
    float value = theModel()->value(cache.item);
    if (tuneValue)
    {
        if (!tuneValue(value, cache.item))
            return;
    }

    Q_ASSERT(value >= 0.f || qFuzzyCompare(value, 0.f));
    Q_ASSERT(value <= 1.f || qFuzzyCompare(value, 1.f));
    value = qBound(0.f, value, 1.f);

    QRect rect = cache.cacheView.rect();
    rect.adjust(2, 2, -2, -2);
    rect.setRight(rect.left() + (int)(value * (float)rect.width()));

    if (rect.width() > 0)
    {
        PainterState state;
        state.save(painter);

        painter->setPen(boundsColor);
        painter->setBrush(contentsColor);
        painter->drawRect(rect);

        state.restore(painter);
    }
}

} // end namespace Qi
