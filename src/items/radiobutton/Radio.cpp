#include "Radio.h"
#include "items/misc/ControllerMousePushableCallback.h"
#include <QStyleOptionButton>

namespace Qi
{

int ModelRadio::compareImpl(const ItemID &left, const ItemID &right) const
{
    if (isRadioItem(left))
        return 1;
    else if (isRadioItem(right))
        return -1;
    else
        return 0;
}

bool ModelRadioCallback::isRadioItemImpl(const ItemID& item) const
{
    Q_ASSERT(isRadioItem);
    return isRadioItem(item);
}

bool ModelRadioCallback::setRadioItemImpl(const ItemID& item)
{
    Q_ASSERT(setRadioItem);

    if (setRadioItem(item))
    {
        emit modelChanged(this);
        return true;
    }

    return false;
}

ModelRadioStorage::ModelRadioStorage(const ItemID& radioItem)
    : m_radioItem(radioItem)
{
}

bool ModelRadioStorage::isRadioItemImpl(const ItemID& item) const
{
    return m_radioItem ==item;
}

bool ModelRadioStorage::setRadioItemImpl(const ItemID& item)
{
    if (m_radioItem == item)
        return false;

    m_radioItem = item;
    emit modelChanged(this);

    return true;
}

ViewRadio::ViewRadio(const QSharedPointer<ModelRadio>& model, bool useDefaultController)
    : ViewModeled<ModelRadio>(model),
      m_pushableTracker(this)
{
    if (useDefaultController)
    {
        setController(createControllerMouseRadio(model));
    }
}

QSize ViewRadio::sizeImpl(const GuiContext& ctx, const ItemID& /*item*/, ViewSizeMode /*sizeMode*/) const
{
    auto style = ctx.style();
    return QSize(style->pixelMetric(QStyle::PM_ExclusiveIndicatorWidth),
                 style->pixelMetric(QStyle::PM_ExclusiveIndicatorHeight));
}

void ViewRadio::drawImpl(QPainter* painter, const GuiContext& ctx, const CacheContext& cache, bool* /*showTooltip*/) const
{
    auto style = ctx.style();

    QStyleOptionButton option;
    ctx.initStyleOption(option);

    // dont initialize styleObject from widget for QWindowsVistaStyle
    // this disables buggous animations
    if (style->inherits("QWindowsVistaStyle"))
        option.styleObject = nullptr;

    option.state |= styleState(cache.item);
    option.rect = cache.cacheView.rect();
    // correct rect
    option.rect = style->subElementRect(QStyle::SE_RadioButtonIndicator, &option, ctx.widget);

    // draw radio button image
    style->drawPrimitive(QStyle::PE_IndicatorRadioButton, &option, painter, ctx.widget);
}

QStyle::State ViewRadio::styleState(const ItemID& item) const
{
    bool isRadioItem = theModel()->isRadioItem(item);

    QStyle::State state = QStyle::State_Enabled | m_pushableTracker.styleStateByItem(item);
    state |= (isRadioItem ? QStyle::State_On : QStyle::State_Off);

    return state;
}

QSharedPointer<ControllerMousePushable> createControllerMouseRadio(const QSharedPointer<ModelRadio>& model)
{
    auto controller = QSharedPointer<ControllerMousePushableCallback>::create();
    controller->onApply = [model] (const ItemID& item, const ControllerContext& /*context*/) {
        Q_ASSERT(item.isValid());
        model->setRadioItem(item);
    };
    return controller;
}

} // end namespace Qi
