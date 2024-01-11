#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/CarInfoService.cpp"

// Mock class for PiRacer
class MockPiRacer : public PiRacer {
public:
    MOCK_METHOD(void, readBatteryInfo, (), (override));
    MOCK_METHOD(float, getBatteryVoltage, (), (override));
    MOCK_METHOD(float, getBatteryCurrent, (), (override));
    MOCK_METHOD(float, getBatteryConsumption, (), (override));
    MOCK_METHOD(float, getBatteryLevel, (), (override));
};

// Test fixture for CarInfoService
class CarInfoServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a mock instance of PiRacer
        piracerMock = std::make_shared<MockPiRacer>();
        // Create an instance of CarInfoStubImpl
        myService = std::make_shared<CarInfoStubImpl>();
        // Set the mock instance of PiRacer in CarInfoStubImpl
        myService->setPiRacer(piracerMock);
    }

    void TearDown() override {
        // Clean up
        piracerMock.reset();
        myService.reset();
    }

    std::shared_ptr<MockPiRacer> piracerMock;
    std::shared_ptr<CarInfoStubImpl> myService;
};

// Test case for registering the service
TEST_F(CarInfoServiceTest, RegisterServiceTest) {
    // Create a mock instance of CommonAPI::Runtime
    std::shared_ptr<CommonAPI::Runtime> runtimeMock = std::make_shared<::testing::NiceMock<MockCommonAPIRuntime>>();
    // Set the mock instance of CommonAPI::Runtime
    CommonAPI::Runtime::setMock(runtimeMock);

    // Expect the registerService function to be called once and return true
    EXPECT_CALL(*runtimeMock, registerService(::testing::_, ::testing::_, myService, ::testing::_))
        .WillOnce(::testing::Return(true));

    // Call the registerService function
    bool result = myService->registerService();

    // Check if the registration was successful
    ASSERT_TRUE(result);
}

// Test case for setting battery info
TEST_F(CarInfoServiceTest, SetBatteryInfoTest) {
    // Set the expected battery info values
    float expectedVoltage = 12.5;
    float expectedCurrent = 2.5;
    float expectedConsumption = 10.0;
    float expectedLevel = 80.0;

    // Expect the corresponding functions to be called and return the expected values
    EXPECT_CALL(*piracerMock, getBatteryVoltage())
        .WillOnce(::testing::Return(expectedVoltage));
    EXPECT_CALL(*piracerMock, getBatteryCurrent())
        .WillOnce(::testing::Return(expectedCurrent));
    EXPECT_CALL(*piracerMock, getBatteryConsumption())
        .WillOnce(::testing::Return(expectedConsumption));
    EXPECT_CALL(*piracerMock, getBatteryLevel())
        .WillOnce(::testing::Return(expectedLevel));

    // Call the setBatteryStruct function
    myService->setBatteryStruct();

    // Check if the battery info was set correctly
    ASSERT_EQ(myService->getBatteryStruct().voltage, expectedVoltage);
    ASSERT_EQ(myService->getBatteryStruct().current, expectedCurrent);
    ASSERT_EQ(myService->getBatteryStruct().consumption, expectedConsumption);
    ASSERT_EQ(myService->getBatteryStruct().level, expectedLevel);
}

// Run all the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}