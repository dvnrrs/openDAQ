#include <opendaq/packet_factory.h>
#include <opendaq/sample_type_traits.h>
#include <opendaq/scaling_ptr.h>
#include <opendaq/data_descriptor_factory.h>
#include <opendaq/data_rule_factory.h>
#include <opendaq/scaling_factory.h>
#include <gtest/gtest.h>

using DataPacketTest = testing::Test;

BEGIN_NAMESPACE_OPENDAQ

// Helper methods

template <typename T, size_t SIZE>
static DataPacketPtr createExplicitPacket(const DataDescriptorPtr& descriptor)
{
    const DataPacketPtr packet = DataPacket(descriptor, SIZE);

    T* data = static_cast<T*>(packet.getRawData());
    for (size_t i = 0; i < SIZE; ++i)
        data[i] = static_cast<T>(i);

    return packet;
}

static DataDescriptorPtr setupDescriptor(SampleType type, DataRulePtr rule, ScalingPtr scaling)
{
    return DataDescriptorBuilder().setSampleType(type).setRule(rule).setPostScaling(scaling).build();
}

template <typename T>
static void validateImplicitLinearDataRulePacket(const DataDescriptorPtr& descriptor, T delta, T start, const NumberPtr& packetOffset)
{
    const DataPacketPtr packet = DataPacket(descriptor, 100, packetOffset);
    const auto scaledData = static_cast<T*>(packet.getData());
    const T offsetT = static_cast<T>(packetOffset);
    for (uint64_t i = 0; i < packet.getSampleCount(); ++i)
        ASSERT_EQ(scaledData[i], static_cast<T>(offsetT + i * delta + start));
}

template <typename T, typename U>
static void validateLinearScalingPacket(const DataDescriptorPtr& descriptor, U scale, U offset)
{
    const DataPacketPtr packet = createExplicitPacket<T, 100>(descriptor);

    const auto scaledData = static_cast<U*>(packet.getData());
    for (uint64_t i = 0; i < packet.getSampleCount(); ++i)
        ASSERT_EQ(scaledData[i], static_cast<U>(i * scale + offset));
}

template <typename T>
static void validateImplicitConstantDataRulePacket(const DataDescriptorPtr& descriptor, T constant)
{
    const DataPacketPtr packet = DataPacket(descriptor, 100);
    const auto scaledData = static_cast<T*>(packet.getData());
    for (uint64_t i = 0; i < packet.getSampleCount(); ++i)
        ASSERT_EQ(scaledData[i], constant);
}

// Tests

TEST_F(DataPacketTest, TestConstructorErrors)
{
    ASSERT_THROW(DataPacket(nullptr, 0, 10), ArgumentNullException);
}

TEST_F(DataPacketTest, DataPacketTestGetters)
{
    const auto desc = setupDescriptor(SampleType::Float64, LinearDataRule(10.5, 200), nullptr);
    const DataPacketPtr packet = DataPacket(desc, 100, 10);

    ASSERT_EQ(packet.getType(), PacketType::Data);

    ASSERT_EQ(packet.getDataDescriptor(), desc);
    ASSERT_EQ(packet.getSampleCount(), 100u);
    ASSERT_EQ(packet.getOffset(), 10);
}


TEST_F(DataPacketTest, DataPacketWithDomainGetters)
{
    const auto desc = setupDescriptor(SampleType::Float64, LinearDataRule(10.5, 200), nullptr);
    const DataPacketPtr domain = DataPacket(desc, 100, 10);
    const DataPacketPtr value = DataPacketWithDomain(domain, desc, 100, 10);

    ASSERT_TRUE(*value.getDomainPacket() == *domain);
}


TEST_F(DataPacketTest, TestDoubleLinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::Float64, LinearDataRule(10.5, 200), nullptr);
    validateImplicitLinearDataRulePacket<double>(descriptor, 10.5, 200, 1000);
}

TEST_F(DataPacketTest, TestSingleLinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::Float32, LinearDataRule(10.5, 0.2), nullptr);
    validateImplicitLinearDataRulePacket<float>(descriptor, static_cast<float>(10.5), static_cast<float>(0.2), 1000);
}

TEST_F(DataPacketTest, TestUInt8LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::UInt8, LinearDataRule(2, 0), nullptr);
    validateImplicitLinearDataRulePacket<uint8_t>(descriptor, 2, 0, 0);
}

TEST_F(DataPacketTest, TestInt8LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::Int8, LinearDataRule(1, 0), nullptr);
    validateImplicitLinearDataRulePacket<int8_t>(descriptor, 1,0, 0);
}

TEST_F(DataPacketTest, TestUInt16LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::UInt16, LinearDataRule(3, 30), nullptr);
    validateImplicitLinearDataRulePacket<uint16_t>(descriptor, 3, 30, 100);
}

TEST_F(DataPacketTest, TestInt16LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::Int16, LinearDataRule(3, 30), nullptr);
    validateImplicitLinearDataRulePacket<int16_t>(descriptor, 3, 30, 100);
}

TEST_F(DataPacketTest, TestUInt32LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::UInt32, LinearDataRule(5, 9), nullptr);
    validateImplicitLinearDataRulePacket<uint32_t>(descriptor, 5, 9, 100);
}

TEST_F(DataPacketTest, TestInt32LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::Int32, LinearDataRule(5, 9), nullptr);
    validateImplicitLinearDataRulePacket<int32_t>(descriptor, 5, 9, 100);
}

TEST_F(DataPacketTest, TestUInt64LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::UInt64, LinearDataRule(10.5, 0), nullptr);
    validateImplicitLinearDataRulePacket<uint64_t>(descriptor, 10, 0, 1000);
}

TEST_F(DataPacketTest, TestInt64LinearDataRule)
{
    const auto descriptor = setupDescriptor(SampleType::Int64, LinearDataRule(10.5, 0), nullptr);
    validateImplicitLinearDataRulePacket<int64_t>(descriptor, 10, 0, 1000);
}

TEST_F(DataPacketTest, TestDoubleLinearScaling)
{
    auto descriptor = setupDescriptor(
        SampleType::Float32,
        ExplicitDataRule(),
        LinearScaling(4.5, 100.2, SampleType::Float64, ScaledSampleType::Float32)
    );
    validateLinearScalingPacket<double, float>(descriptor, static_cast<float>(4.5), static_cast<float>(100.2));

    descriptor = setupDescriptor(
        SampleType::Float64,
        ExplicitDataRule(),
        LinearScaling(4.5, 100.2, SampleType::Float64, ScaledSampleType::Float64)
    );
    validateLinearScalingPacket<double, double>(descriptor, 4.5, 100.2);
}

TEST_F(DataPacketTest, TestUInt8LinearScaling)
{
    auto descriptor = setupDescriptor(
        SampleType::Float32,
        ExplicitDataRule(),
        LinearScaling(2, 5, SampleType::UInt8, ScaledSampleType::Float32)
    );
    validateLinearScalingPacket<uint8_t, float>(descriptor, 2, 5);

    descriptor = setupDescriptor(
        SampleType::Float64,
        ExplicitDataRule(),
        LinearScaling(2, 5, SampleType::UInt8, ScaledSampleType::Float64)
    );
    validateLinearScalingPacket<uint8_t, double>(descriptor, 2, 5);
}

TEST_F(DataPacketTest, TestInt8LinearScaling)
{
    auto descriptor = setupDescriptor(
        SampleType::Float32,
        ExplicitDataRule(),
        LinearScaling(1, 0, SampleType::Int8, ScaledSampleType::Float32)
    );
    validateLinearScalingPacket<int8_t, float>(descriptor, 1, 0);

    descriptor = setupDescriptor(
        SampleType::Float64,
        ExplicitDataRule(),
        LinearScaling(1, 0, SampleType::Int8, ScaledSampleType::Float64)
    );
    validateLinearScalingPacket<int8_t, double>(descriptor, 1, 0);
}

TEST_F(DataPacketTest, TestUInt16LinearScaling)
{
    auto descriptor =
        setupDescriptor(SampleType::Float32, ExplicitDataRule(), LinearScaling(4, 15, SampleType::UInt16, ScaledSampleType::Float32));
    validateLinearScalingPacket<uint16_t, float>(descriptor, 4, 15);

    descriptor =
        setupDescriptor(SampleType::Float64, ExplicitDataRule(), LinearScaling(4, 15, SampleType::UInt16, ScaledSampleType::Float64));
    validateLinearScalingPacket<uint16_t, double>(descriptor, 4, 15);
}

TEST_F(DataPacketTest, TestInt16LinearScaling)
{
    auto descriptor = setupDescriptor(
        SampleType::Float32,
        ExplicitDataRule(),
        LinearScaling(4, 15, SampleType::Int16, ScaledSampleType::Float32)
    );
    validateLinearScalingPacket<int16_t, float>(descriptor, 4, 15);

    descriptor = setupDescriptor(
        SampleType::Float64,
        ExplicitDataRule(),
        LinearScaling(4, 15, SampleType::Int16, ScaledSampleType::Float64)
    );
    validateLinearScalingPacket<int16_t, double>(descriptor, 4, 15);
}

TEST_F(DataPacketTest, TestUInt32LinearScaling)
{
    auto descriptor = setupDescriptor(SampleType::Float32, ExplicitDataRule(), LinearScaling(102, 1000, SampleType::UInt32, ScaledSampleType::Float32));
    validateLinearScalingPacket<uint32_t, float>(descriptor, 102, 1000);

    descriptor = setupDescriptor(SampleType::Float64, ExplicitDataRule(), LinearScaling(102, 1000, SampleType::UInt32, ScaledSampleType::Float64));
    validateLinearScalingPacket<uint32_t, double>(descriptor, 102, 1000);
}

TEST_F(DataPacketTest, TestInt32LinearScaling)
{
    auto descriptor = setupDescriptor(
        SampleType::Float32,
        ExplicitDataRule(),
        LinearScaling(102, 1000, SampleType::Int32, ScaledSampleType::Float32)
    );
    validateLinearScalingPacket<int32_t, float>(descriptor, 102, 1000);

    descriptor = setupDescriptor(
        SampleType::Float64,
        ExplicitDataRule(),
        LinearScaling(102, 1000, SampleType::Int32, ScaledSampleType::Float64)
    );
    validateLinearScalingPacket<int32_t, double>(descriptor, 102, 1000);
}

TEST_F(DataPacketTest, TestUInt64LinearScaling)
{
    auto descriptor = setupDescriptor(SampleType::Float32, ExplicitDataRule(), LinearScaling(1012, 10020, SampleType::UInt64, ScaledSampleType::Float32));
    validateLinearScalingPacket<uint64_t, float>(descriptor, 1012, 10020);

    descriptor = setupDescriptor(SampleType::Float64, ExplicitDataRule(), LinearScaling(1012, 10020, SampleType::UInt64, ScaledSampleType::Float64));
    validateLinearScalingPacket<uint64_t, double>(descriptor, 1012, 10020);
}

TEST_F(DataPacketTest, TestInt64LinearScaling)
{
    auto descriptor = setupDescriptor(
        SampleType::Float32,
        ExplicitDataRule(),
        LinearScaling(1012, 10020, SampleType::Int64, ScaledSampleType::Float32)
    );
    validateLinearScalingPacket<int64_t, float>(descriptor, 1012, 10020);

    descriptor = setupDescriptor(
        SampleType::Float64,
        ExplicitDataRule(),
        LinearScaling(1012, 10020, SampleType::Int64, ScaledSampleType::Float64)
    );
    validateLinearScalingPacket<int64_t, double>(descriptor, 1012, 10020);
}

TEST_F(DataPacketTest, TestConstantRule)
{
    auto descriptor = setupDescriptor(SampleType::Int32, ConstantDataRule(111), nullptr);
    validateImplicitConstantDataRulePacket<int32_t>(descriptor, 111);

    descriptor = setupDescriptor(SampleType::UInt8, ConstantDataRule(123), nullptr);
    validateImplicitConstantDataRulePacket<uint8_t>(descriptor, 123);

    descriptor = setupDescriptor(SampleType::Float32, ConstantDataRule(20.5), nullptr);
    validateImplicitConstantDataRulePacket<float>(descriptor, static_cast<float>(20.5));

    descriptor = setupDescriptor(SampleType::Float64, ConstantDataRule(678.2), nullptr);
    validateImplicitConstantDataRulePacket<double>(descriptor, 678.2);
}

TEST_F(DataPacketTest, TestRangeType)
{
    RangeType<uint64_t> t1(10, 20);
    RangeType<uint64_t> t2(10, 20);
    RangeType<uint64_t> t3(20, 10);

    ASSERT_TRUE(t1 == t2);
    ASSERT_FALSE(t1 != t2);
    ASSERT_FALSE(t1 == t3);
    ASSERT_TRUE(t1 != t3);
}

TEST_F(DataPacketTest, PacketEqualsExplicit)
{
    auto descriptor = setupDescriptor(SampleType::UInt16, ExplicitDataRule(), nullptr);
    auto packet1 = createExplicitPacket<uint16_t, 100>(descriptor);
    auto packet2 = createExplicitPacket<uint16_t, 100>(descriptor);
    auto packet3 = createExplicitPacket<uint16_t, 50>(descriptor);

    ASSERT_EQ(packet1, packet2);
    ASSERT_NE(packet1, packet3);
}

TEST_F(DataPacketTest, PacketEqualsExplicitWithPostScale)
{
    auto descriptor =
        setupDescriptor(SampleType::Float64, ExplicitDataRule(), LinearScaling(4, 15, SampleType::UInt16, ScaledSampleType::Float64));
    auto packet1 = createExplicitPacket<uint16_t, 100>(descriptor);
    auto packet2 = createExplicitPacket<uint16_t, 100>(descriptor);
    auto packet3 = createExplicitPacket<uint16_t, 50>(descriptor);

    ASSERT_EQ(packet1, packet2);
    ASSERT_NE(packet1, packet3);
}

TEST_F(DataPacketTest, PacketEqualsImplicit)
{
    auto descriptor = setupDescriptor(SampleType::UInt64, LinearDataRule(1, 0), nullptr);
    auto packet1 = DataPacket(descriptor, 100, 100);
    auto packet2 = DataPacket(descriptor, 100, 100);
    auto packet3 = DataPacket(descriptor, 100, 50);
    auto packet4 = DataPacket(descriptor, 50, 100);

    ASSERT_EQ(packet1, packet2);
    ASSERT_NE(packet1, packet3);
    ASSERT_NE(packet1, packet4);
}

TEST_F(DataPacketTest, PacketEqualsMixTypes)
{
    auto packetExplicit = createExplicitPacket<uint16_t, 100>(
        setupDescriptor(SampleType::Float64, ExplicitDataRule(), nullptr)
    );
    auto packetExplicitWithPostScaling = createExplicitPacket<uint16_t, 100>(
        setupDescriptor(SampleType::Float64, ExplicitDataRule(),
        LinearScaling(4, 15, SampleType::UInt16, ScaledSampleType::Float64))
    );
    auto packetimplicit = DataPacket(
        setupDescriptor(SampleType::UInt64, LinearDataRule(1, 0), nullptr),
        100,
        100
    );

    ASSERT_NE(packetExplicit, packetExplicitWithPostScaling);
    ASSERT_NE(packetExplicit, packetimplicit);
    ASSERT_NE(packetExplicitWithPostScaling, packetimplicit);
}

TEST_F(DataPacketTest, PacketRefCount)
{
    const auto desc = setupDescriptor(SampleType::Float64, LinearDataRule(10.5, 200), nullptr);
    const DataPacketPtr packet = DataPacket(desc, 100, 10);

    ASSERT_EQ(packet.getRefCount(), 1u);
}

END_NAMESPACE_OPENDAQ
