#include "Avioos/OSP/Web/WebServerDomainDispatcher.h"
namespace Avioos
{
namespace OSP
{
namespace Web
{
const std::type_info &WebServerDomainDispatcher::type() const
{
    return typeid(WebServerDomainDispatcher);
}

bool WebServerDomainDispatcher::isA(const std::type_info &otherType) const
{
    std::string name(typeid(WebServerDomainDispatcher).name());

    return name == otherType.name() || Poco::OSP::Service::isA(otherType);
}
} // namespace Web
} // namespace OSP
} // namespace Avioos