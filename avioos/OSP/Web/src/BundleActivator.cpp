#include "Poco/OSP/BundleActivator.h"
#include "Poco/OSP/BundleContext.h"
#include "Poco/ClassLibrary.h"
namespace Avioos
{
class BundleActivator : public Poco::OSP::BundleActivator
{
  public:
    void start(Poco::OSP::BundleContext::Ptr pContext)
    {
      //
    }
    void stop(Poco::OSP::BundleContext::Ptr pContext)
    {
      //
    }
};
} // namespace Avioos
POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
POCO_EXPORT_CLASS(Avioos::BundleActivator)
POCO_END_MANIFEST