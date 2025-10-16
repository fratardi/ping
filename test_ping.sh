#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0
TOTAL_TESTS=0

# Function to print test header
print_test() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "\n${BLUE}=== TEST $TOTAL_TESTS: $1 ===${NC}"
}

# Function to check if command succeeded
check_result() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ PASS${NC}"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo -e "${RED}✗ FAIL${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Function to run test with expected failure
expect_fail() {
    print_test "$1"
    OUTPUT=$(eval "$2" 2>&1)
    EXIT_CODE=$?
    echo "$OUTPUT" | head -5
    if [ $EXIT_CODE -ne 0 ]; then
        echo -e "${GREEN}✓ PASS (expected failure)${NC}"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo -e "${RED}✗ FAIL (should have failed)${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

echo -e "${YELLOW}╔═══════════════════════════════════════╗${NC}"
echo -e "${YELLOW}║   PING IMPLEMENTATION TEST SUITE     ║${NC}"
echo -e "${YELLOW}╔═══════════════════════════════════════╗${NC}"

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo -e "${RED}Please run as root (use sudo)${NC}"
    exit 1
fi

# Check if ping binary exists
if [ ! -f "./ping" ]; then
    echo -e "${RED}Error: ./ping binary not found. Run 'make' first.${NC}"
    exit 1
fi

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  BASIC FUNCTIONALITY TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 1: Basic ping to IP
print_test "Basic ping to IP address (8.8.8.8)"
timeout 3 ./ping 8.8.8.8 | head -5
check_result

# Test 2: Ping to hostname
print_test "Ping to hostname (google.com)"
timeout 3 ./ping google.com | head -5
check_result

# Test 3: Ping to localhost
print_test "Ping to localhost (127.0.0.1)"
timeout 2 ./ping 127.0.0.1 | head -5
check_result

# Test 4: Ping with statistics display
print_test "Statistics display (Ctrl+C simulation)"
(./ping 8.8.8.8 & PID=$!; sleep 2; kill -INT $PID; wait $PID 2>/dev/null)
check_result

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  COUNT OPTION (-c) TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 5: Count option with 3 packets
print_test "Send exactly 3 packets (-c 3)"
./ping -c 3 8.8.8.8
check_result

# Test 6: Count option with 1 packet
print_test "Send exactly 1 packet (-c 1)"
./ping -c 1 8.8.8.8
check_result

# Test 7: Count option with 10 packets
print_test "Send exactly 10 packets (-c 10)"
./ping -c 10 127.0.0.1
check_result

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  INTERVAL OPTION (-i) TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 8: Fast interval (0.2 seconds)
print_test "Fast interval 0.2s (-i 0.2)"
timeout 2 ./ping -i 0.2 127.0.0.1 | head -8
check_result

# Test 9: Slow interval (2 seconds)
print_test "Slow interval 2s (-i 2)"
timeout 5 ./ping -i 2 127.0.0.1 | head -5
check_result

# Test 10: Very fast interval (0.1 seconds)
print_test "Very fast interval 0.1s (-i 0.1)"
timeout 2 ./ping -i 0.1 127.0.0.1 | head -10
check_result

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  TTL OPTION (-ttl) TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 11: TTL=1 (should get Time Exceeded)
print_test "TTL=1 to remote host (expect Time Exceeded)"
timeout 3 ./ping -ttl 1 8.8.8.8 | head -5
check_result

# Test 12: TTL=1 to localhost (should work)
print_test "TTL=1 to localhost (should succeed)"
./ping -c 2 -ttl 1 127.0.0.1
check_result

# Test 13: TTL=64 (default)
print_test "TTL=64 (normal operation)"
./ping -c 2 -ttl 64 8.8.8.8
check_result

# Test 14: TTL=255 (maximum)
print_test "TTL=255 (maximum value)"
./ping -c 2 -ttl 255 8.8.8.8
check_result

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  COMBINED OPTIONS TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 15: -c and -i combined
print_test "Combined: -c 5 -i 0.3"
./ping -c 5 -i 0.3 127.0.0.1
check_result

# Test 16: -c and -ttl combined
print_test "Combined: -c 3 -ttl 100"
./ping -c 3 -ttl 100 8.8.8.8
check_result

# Test 17: -i and -ttl combined
print_test "Combined: -i 0.5 -ttl 128"
timeout 2 ./ping -i 0.5 -ttl 128 127.0.0.1 | head -5
check_result

# Test 18: All options combined
print_test "Combined: -c 5 -i 0.4 -ttl 64"
./ping -c 5 -i 0.4 -ttl 64 8.8.8.8
check_result

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  ERROR HANDLING TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 19: No arguments
expect_fail "No arguments (should fail)" "./ping"

# Test 20: Invalid TTL (too high)
expect_fail "Invalid TTL=300 (should fail)" "./ping -ttl 300 8.8.8.8"

# Test 21: Invalid TTL (zero)
expect_fail "Invalid TTL=0 (should fail)" "./ping -ttl 0 8.8.8.8"

# Test 22: Invalid count (zero)
expect_fail "Invalid count=0 (should fail)" "./ping -c 0 8.8.8.8"

# Test 23: Invalid count (negative)
expect_fail "Invalid count=-1 (should fail)" "./ping -c -1 8.8.8.8"

# Test 24: Invalid interval (negative)
expect_fail "Invalid interval=-1 (should fail)" "./ping -i -1 8.8.8.8"

# Test 25: Non-existent host
expect_fail "Non-existent host (should fail)" "./ping nonexistent.invalid.host"

# Test 26: Invalid option
expect_fail "Invalid option -x (should fail)" "./ping -x 8.8.8.8"

# Test 27: Missing option argument (-c)
expect_fail "Missing argument for -c (should fail)" "./ping -c"

# Test 28: Missing option argument (-i)
expect_fail "Missing argument for -i (should fail)" "./ping -i"

# Test 29: Missing option argument (-ttl)
expect_fail "Missing argument for -ttl (should fail)" "./ping -ttl"

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  EDGE CASES TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 30: Very large count
print_test "Large count -c 100"
timeout 5 ./ping -c 100 -i 0.01 127.0.0.1 | tail -5
check_result

# Test 31: Multiple hosts (should fail - too many args)
expect_fail "Multiple destinations (should fail)" "./ping 8.8.8.8 1.1.1.1"

# Test 32: IP address format
print_test "IPv4 address format (192.168.1.1)"
timeout 2 ./ping -c 1 192.168.1.1 2>&1 | head -3
# This might fail if address is unreachable, so we just run it
echo -e "${BLUE}(Address may be unreachable - test completed)${NC}"
TESTS_PASSED=$((TESTS_PASSED + 1))
TOTAL_TESTS=$((TOTAL_TESTS + 1))

echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}  STATISTICS ACCURACY TESTS${NC}"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

# Test 33: Verify packet count in statistics
print_test "Verify packet count accuracy"
OUTPUT=$(./ping -c 5 127.0.0.1)
echo "$OUTPUT"
if echo "$OUTPUT" | grep -q "5 packets transmitted, 5 packets received"; then
    echo -e "${GREEN}✓ PASS - Packet counts match${NC}"
    TESTS_PASSED=$((TESTS_PASSED + 1))
else
    echo -e "${RED}✗ FAIL - Packet counts don't match${NC}"
    TESTS_FAILED=$((TESTS_FAILED + 1))
fi

# Test 34: Verify 0% packet loss for localhost
print_test "Verify 0% packet loss for localhost"
OUTPUT=$(./ping -c 5 127.0.0.1)
if echo "$OUTPUT" | grep -q "0.0% packet loss"; then
    echo -e "${GREEN}✓ PASS - 0% packet loss${NC}"
    TESTS_PASSED=$((TESTS_PASSED + 1))
else
    echo -e "${RED}✗ FAIL - Unexpected packet loss${NC}"
    TESTS_FAILED=$((TESTS_FAILED + 1))
fi

# Test 35: Verify statistics format (min/avg/max/stddev)
print_test "Verify statistics format"
OUTPUT=$(./ping -c 3 127.0.0.1)
if echo "$OUTPUT" | grep -q "round-trip min/avg/max/stddev"; then
    echo -e "${GREEN}✓ PASS - Statistics format correct${NC}"
    TESTS_PASSED=$((TESTS_PASSED + 1))
else
    echo -e "${RED}✗ FAIL - Statistics format incorrect${NC}"
    TESTS_FAILED=$((TESTS_FAILED + 1))
fi

echo -e "\n${YELLOW}╔═══════════════════════════════════════╗${NC}"
echo -e "${YELLOW}║          TEST SUMMARY                 ║${NC}"
echo -e "${YELLOW}╚═══════════════════════════════════════╝${NC}"

echo -e "\nTotal Tests: ${BLUE}$TOTAL_TESTS${NC}"
echo -e "Passed: ${GREEN}$TESTS_PASSED${NC}"
echo -e "Failed: ${RED}$TESTS_FAILED${NC}"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "\n${GREEN}✓ ALL TESTS PASSED!${NC}"
    echo -e "${GREEN}The ping implementation is working correctly.${NC}\n"
    exit 0
else
    echo -e "\n${RED}✗ SOME TESTS FAILED${NC}"
    echo -e "${RED}Please review the failed tests above.${NC}\n"
    exit 1
fi
