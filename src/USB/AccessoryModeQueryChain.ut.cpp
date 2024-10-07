/*
*  This file is part of aasdk library project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  aasdk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  aasdk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with aasdk. If not, see <http://www.gnu.org/licenses/>.
*/

#include <aasdk/USB/UT/USBWrapper.mock.hpp>
#include <aasdk/USB/UT/AccessoryModeQueryFactory.mock.hpp>
#include <aasdk/USB/UT/AccessoryModeQueryChainPromiseHandler.mock.hpp>
#include <aasdk/USB/UT/AccessoryModeQuery.mock.hpp>
#include <aasdk/USB/AccessoryModeQueryChain.hpp>


namespace aasdk
{
namespace usb
{
namespace ut
{

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::NotNull;

class AccessoryModeQueryChainUnitTest : public testing::Test
{
protected:
    AccessoryModeQueryChainUnitTest()
        : deviceHandle_(reinterpret_cast<libusb_device_handle*>(&dummyDeviceHandle_), [](auto*) {})
        , queryMock_(std::make_shared<AccessoryModeQueryMock>())
        , promise_(IAccessoryModeQueryChain::Promise::defer(ioService_))
    {
        promise_->then(std::bind(&AccessoryModeQueryChainPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                      std::bind(&AccessoryModeQueryChainPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));
    }

    boost::asio::io_service ioService_;
    USBWrapperMock usbWrapperMock_;
    AccessoryModeQueryFactoryMock queryFactoryMock_;
    USBWrapperMock::DummyDeviceHandle dummyDeviceHandle_;
    DeviceHandle deviceHandle_;
    std::shared_ptr<AccessoryModeQueryMock> queryMock_;
    AccessoryModeQueryChainPromiseHandlerMock promiseHandlerMock_;
    IAccessoryModeQueryChain::Promise::Pointer promise_;
};

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_QueryAOAPDevice)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MODEL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_DESCRIPTION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_VERSION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_URI, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_SERIAL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::START, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(promiseHandlerMock_, onResolve(deviceHandle_));
    EXPECT_CALL(promiseHandlerMock_, onReject(_)).Times(0);
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_ProtocolVersionQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_AOAP_PROTOCOL_VERSION);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_ManufacturerQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_ModelQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MODEL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_DescriptionQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MODEL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_DESCRIPTION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_VersionQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MODEL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_DESCRIPTION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_VERSION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_URIQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MODEL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_DESCRIPTION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_VERSION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_URI, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_SerialQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MODEL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_DESCRIPTION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_VERSION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_URI, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_SERIAL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_StartQueryFailed)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;

    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MODEL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_DESCRIPTION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_VERSION, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_URI, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_SERIAL, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::START, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_Cancel)
{
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));

    IUSBEndpoint::Pointer usbEndpoint;
    IAccessoryModeQuery::Promise::Pointer queryPromise;
    EXPECT_CALL(*queryMock_, start(_)).WillRepeatedly(SaveArg<0>(&queryPromise));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(DoAll(SaveArg<1>(&usbEndpoint), Return(queryMock_)));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    queryPromise->resolve(usbEndpoint);
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::SEND_MANUFACTURER, usbEndpoint)).WillOnce(Return(queryMock_));
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(*queryMock_, cancel());
    queryChain->cancel();

    const error::Error e(error::ErrorCode::OPERATION_ABORTED);
    queryPromise->reject(e);
    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(e));
    ioService_.run();
}

TEST_F(AccessoryModeQueryChainUnitTest, AccessoryModeQueryChain_RejectWhenInProgress)
{
    EXPECT_CALL(*queryMock_, start(_));
    EXPECT_CALL(queryFactoryMock_, createQuery(AccessoryModeQueryType::PROTOCOL_VERSION, _)).WillOnce(Return(queryMock_));
    AccessoryModeQueryChain::Pointer queryChain(std::make_shared<AccessoryModeQueryChain>(usbWrapperMock_, ioService_, queryFactoryMock_));
    queryChain->start(deviceHandle_, std::move(promise_));

    ioService_.run();
    ioService_.reset();

    auto secondPromise = IAccessoryModeQueryChain::Promise::defer(ioService_);
    secondPromise->then(std::bind(&AccessoryModeQueryChainPromiseHandlerMock::onResolve, &promiseHandlerMock_, std::placeholders::_1),
                       std::bind(&AccessoryModeQueryChainPromiseHandlerMock::onReject, &promiseHandlerMock_, std::placeholders::_1));

    EXPECT_CALL(promiseHandlerMock_, onResolve(_)).Times(0);
    EXPECT_CALL(promiseHandlerMock_, onReject(error::Error(error::ErrorCode::OPERATION_IN_PROGRESS)));
    queryChain->start(deviceHandle_, std::move(secondPromise));

    ioService_.run();
}

}
}
}
