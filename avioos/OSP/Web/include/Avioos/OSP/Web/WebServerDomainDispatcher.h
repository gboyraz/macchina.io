#ifndef Avioos_OSP_Web_WebServerDomainDispatcher_INCLUDED
#define Avioos_OSP_Web_WebServerDomainDispatcher_INCLUDED

#include "Avioos/OSP/Web/Web.h"
#include "Poco/OSP/Service.h"
#include "Poco/AutoPtr.h"

namespace Avioos
{
namespace OSP
{
namespace Web
{
class AvioosOSPWeb_API WebServerDomainDispatcher : public Poco::OSP::Service
{
    typedef Poco::AutoPtr<WebServerDomainDispatcher> Ptr;

    virtual const std::type_info& type() const;
    virtual bool isA(const std::type_info& otherType) const;
};
} // namespace Web
} // namespace OSP
} // namespace Avioos
#endif