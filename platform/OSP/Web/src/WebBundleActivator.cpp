//
// WebBundleActivator.cpp
//
// Copyright (c) 2007-2014, Applied Informatics Software Engineering GmbH.
// All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//


#include "Poco/OSP/BundleActivator.h"
#include "Poco/OSP/BundleContext.h"
#include "Poco/OSP/Bundle.h"
#include "Poco/OSP/ServiceRegistry.h"
#include "Poco/OSP/PreferencesService.h"
#include "Poco/OSP/ExtensionPointService.h"
#include "Poco/OSP/ServiceListener.h"
#include "Poco/OSP/Web/MediaTypeMapper.h"
#include "Poco/OSP/Web/WebServerDispatcher.h"
#include "Poco/OSP/Web/WebSessionManager.h"
#include "Poco/OSP/Web/WebSessionStore.h"
#include "Poco/OSP/Web/WebServerExtensionPoint.h"
#include "Poco/OSP/Web/WebFilterExtensionPoint.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Format.h"
#include "Poco/AutoPtr.h"
#include "Poco/Delegate.h"
#include "Poco/ClassLibrary.h"
#include <memory>
#include <istream>


using Poco::OSP::BundleActivator;
using Poco::OSP::BundleContext;
using Poco::OSP::Bundle;
using Poco::OSP::Service;
using Poco::OSP::ServiceRef;
using Poco::OSP::PreferencesService;
using Poco::OSP::Properties;
using Poco::OSP::ExtensionPointService;
using Poco::OSP::Web::MediaTypeMapper;
using Poco::OSP::Web::WebServerDispatcher;
using Poco::OSP::Web::WebSessionManager;
using Poco::OSP::Web::WebSessionStore;
using Poco::OSP::Web::WebServerExtensionPoint;
using Poco::OSP::Web::WebFilterExtensionPoint;


class WebBundleActivator: public BundleActivator
	/// The BundleActivator for the Web Bundle.
	///
	/// Registers the MediaTypeMapper service, the WebSessionManager service
	/// and the WebServerDispatcher service and installs the WebServerExtensionPoint.
{
public:
	WebBundleActivator()
	{
	}

	~WebBundleActivator()
	{
	}

	void start(BundleContext::Ptr pContext)
	{
		_pContext = pContext;

		MediaTypeMapper::Ptr pMediaTypeMapper = new MediaTypeMapper;
#if __cplusplus < 201103L
		std::auto_ptr<std::istream> pStream(pContext->thisBundle()->getResource("mime.types"));
#else
		std::unique_ptr<std::istream> pStream(pContext->thisBundle()->getResource("mime.types"));
#endif
		if (pStream.get())
		{
			pMediaTypeMapper->load(*pStream);
		}
		_pMediaTypeMapperSvc = pContext->registry().registerService(MediaTypeMapper::SERVICE_NAME, pMediaTypeMapper, Properties());
		ServiceRef::Ptr pPrefsSvcRef = pContext->registry().findByName("osp.core.preferences");

		std::string authServiceName(pContext->thisBundle()->properties().getString("authServiceName", ""));
		bool cacheResources(pContext->thisBundle()->properties().getBool("cacheResources", false));
		bool compressResponse(pContext->thisBundle()->properties().getBool("compressResponses", false));
		std::string compressedMediaTypesString(pContext->thisBundle()->properties().getString("compressedMediaTypes", ""));
		std::string sessionCookiePersistence(pContext->thisBundle()->properties().getString("cookiePersistence", "persistent"));
		bool sessionCookieSecure(pContext->thisBundle()->properties().getBool("cookieSecure", false));
		std::string csrfCookie(pContext->thisBundle()->properties().getString("csrfCookie", "XSRF-TOKEN"));
		bool csrfProtectionEnabled(pContext->thisBundle()->properties().getBool("csrfProtectionEnabled", true));
		bool verifyAddress(pContext->thisBundle()->properties().getBool("verifyAddress", true));
		std::string webSessionStore;
		if (pPrefsSvcRef)
		{
			Poco::AutoPtr<PreferencesService> pPrefsSvc = pPrefsSvcRef->castedInstance<PreferencesService>();
			authServiceName = pPrefsSvc->configuration()->getString("osp.web.authServiceName", authServiceName);
			cacheResources = pPrefsSvc->configuration()->getBool("osp.web.cacheResources", cacheResources);
			compressResponse = pPrefsSvc->configuration()->getBool("osp.web.compressResponses", compressResponse);
			compressedMediaTypesString = pPrefsSvc->configuration()->getString("osp.web.compressedMediaTypes", compressedMediaTypesString);
			sessionCookiePersistence = pPrefsSvc->configuration()->getString("osp.web.sessionManager.cookiePersistence", sessionCookiePersistence);
			sessionCookieSecure = pPrefsSvc->configuration()->getBool("osp.web.sessionManager.cookieSecure", sessionCookieSecure);
			csrfCookie = pPrefsSvc->configuration()->getString("osp.web.sessionManager.csrfCookie", csrfCookie);
			csrfProtectionEnabled = pPrefsSvc->configuration()->getBool("osp.web.csrfProtectionEnabled", csrfProtectionEnabled);
			verifyAddress = pPrefsSvc->configuration()->getBool("osp.web.sessionManager.verifyAddress", verifyAddress);
			webSessionStore = pPrefsSvc->configuration()->getString("osp.web.sessionManager.sessionStore", webSessionStore);
		}

		Poco::StringTokenizer tok(compressedMediaTypesString, ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
		std::set<std::string> compressedMediaTypes(tok.begin(), tok.end());

		Poco::AutoPtr<WebServerDispatcher> pWebServerDispatcher = new WebServerDispatcher(pContext, pMediaTypeMapper, authServiceName, compressResponse, compressedMediaTypes, cacheResources);
		_pWebServerDispatcherSvc = pContext->registry().registerService(WebServerDispatcher::SERVICE_NAME, pWebServerDispatcher, Properties());

		WebSessionManager::CookiePersistence cookiePersistence = WebSessionManager::COOKIE_PERSISTENT;
		if (sessionCookiePersistence == "transient")
		{
			cookiePersistence = WebSessionManager::COOKIE_TRANSIENT;
		}
		else if (sessionCookiePersistence != "persistent")
		{
			pContext->logger().warning(Poco::format("Ignoring invalid value for osp.web.sessionManager.cookiePersistence: '%s'. Valid values are 'transient' or 'persistent'.", sessionCookiePersistence));
		}

		_pWebSessionManager = new WebSessionManager(pContext);
		_pWebSessionManager->setCookiePersistence(cookiePersistence);
		_pWebSessionManager->setCookieSecure(sessionCookieSecure);
		_pWebSessionManager->setVerifyAddress(verifyAddress);

		if (csrfProtectionEnabled)
		{
			_pWebSessionManager->setCSRFCookie(csrfCookie);
		}

		_pWebSessionManagerSvc = pContext->registry().registerService(WebSessionManager::SERVICE_NAME, _pWebSessionManager, Properties());

		ServiceRef::Ptr pXPSRef = pContext->registry().findByName("osp.core.xp");
		if (pXPSRef)
		{
			Poco::AutoPtr<ExtensionPointService> pXPS = pXPSRef->castedInstance<ExtensionPointService>();
			_pWebServerExtensionPoint = new WebServerExtensionPoint(pContext, pWebServerDispatcher);
			pXPS->registerExtensionPoint(pContext->thisBundle(), WebServerExtensionPoint::EXTPOINT_DIRECTORY, _pWebServerExtensionPoint);
			pXPS->registerExtensionPoint(pContext->thisBundle(), WebServerExtensionPoint::EXTPOINT_HANDLER, _pWebServerExtensionPoint);
			_pWebFilterExtensionPoint = new WebFilterExtensionPoint(pContext, pWebServerDispatcher);
			pXPS->registerExtensionPoint(pContext->thisBundle(), WebFilterExtensionPoint::EXTPOINT_FILTER, _pWebFilterExtensionPoint);
		}
		else
		{
			pContext->logger().error("The ExtensionPointService is not available.");
		}

		if (!webSessionStore.empty())
		{
			_pStoreListener = pContext->registry().createListener(Poco::format("name == \"%s\"", webSessionStore),
				Poco::delegate(this, &WebBundleActivator::onStoreRegistered),
				Poco::delegate(this, &WebBundleActivator::onStoreUnregistered));
		}
	}

	void stop(BundleContext::Ptr pContext)
	{
		_pContext = 0;
		_pStoreListener = 0;
		pContext->registry().unregisterService(_pWebSessionManagerSvc);
		pContext->registry().unregisterService(_pWebServerDispatcherSvc);
		pContext->registry().unregisterService(_pMediaTypeMapperSvc);
		_pWebSessionManager = 0;
		_pWebSessionManagerSvc = 0;
		_pWebServerDispatcherSvc = 0;
		_pMediaTypeMapperSvc = 0;
		_pWebServerExtensionPoint = 0;
		_pWebFilterExtensionPoint = 0;
	}

protected:
	void onStoreRegistered(const Poco::OSP::ServiceRef::Ptr& pStoreRef)
	{
		_pContext->logger().information("WebSessionStore registered: %s", pStoreRef->name());
		WebSessionStore::Ptr pStore = pStoreRef->castedInstance<WebSessionStore>();
		_pWebSessionManager->setSessionStore(pStore);
	}

	void onStoreUnregistered(const Poco::OSP::ServiceRef::Ptr& pStoreRef)
	{
		_pContext->logger().information("WebSessionStore unregistered: %s", pStoreRef->name());
		if (_pWebSessionManager)
		{
			_pWebSessionManager->setSessionStore(0);
		}
	}

private:
	Poco::OSP::BundleContext::Ptr _pContext;
	Poco::OSP::ServiceListener::Ptr _pStoreListener;
	Poco::AutoPtr<WebSessionManager> _pWebSessionManager;
	ServiceRef::Ptr _pMediaTypeMapperSvc;
	ServiceRef::Ptr _pWebServerDispatcherSvc;
	ServiceRef::Ptr _pWebSessionManagerSvc;
	Poco::AutoPtr<WebServerExtensionPoint> _pWebServerExtensionPoint;
	Poco::AutoPtr<WebFilterExtensionPoint> _pWebFilterExtensionPoint;
};


POCO_BEGIN_MANIFEST(BundleActivator)
	POCO_EXPORT_CLASS(WebBundleActivator)
POCO_END_MANIFEST
