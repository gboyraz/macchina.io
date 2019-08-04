//
// BundleActivator.cpp
//
// Copyright (c) 2018, Avioos LLC.
// All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//

#include "Poco/OSP/BundleActivator.h"
#include "Poco/OSP/BundleContext.h"
#include "Poco/ClassLibrary.h"
#include "Poco/OSP/ServiceRegistry.h"
#include "Poco/OSP/ServiceRef.h"

namespace Avioos
{

class BundleActivator : public Poco::OSP::BundleActivator
{
  public:
	void start(Poco::OSP::BundleContext::Ptr pContext)
	{
		pContext->logger().information("Bundle started.");

		// available services on startup
		Poco::OSP::ServiceRegistry &registry = pContext->registry();
		std::vector<Poco::OSP::ServiceRef::Ptr> results;
		registry.find("name =~ \"*\"", results);
		for (auto it = results.cbegin(); it != results.cend(); it++)
		{
			Poco::OSP::ServiceRef::Ptr pServiceRef = *it;
			// pServiceRef->name();
			// pServiceRef->properties();
			// pServiceRef->instance();

			pContext->logger().information(pServiceRef->name());
		}
	}
	
	void stop(Poco::OSP::BundleContext::Ptr pContext)
	{
		pContext->logger().information("Bundle is stopping.");
	}
};

} // namespace Avioos

POCO_BEGIN_MANIFEST(Poco::OSP::BundleActivator)
POCO_EXPORT_CLASS(Avioos::BundleActivator)
POCO_END_MANIFEST