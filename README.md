# ft_ping - ICMP Ping Implementation

A complete implementation of the `ping` utility using raw ICMP sockets in C. This project provides network diagnostic capabilities by sending ICMP ECHO_REQUEST packets to network hosts and displaying the responses.

## Features

### Mandatory Features
- ✅ ICMP Echo Request/Reply implementation using `SOCK_RAW`
- ✅ Manual ICMP checksum calculation
- ✅ IP header parsing for received packets
- ✅ Hostname resolution (DNS)
- ✅ Real-time packet response display
- ✅ Comprehensive statistics (min/avg/max/stddev RTT, packet loss)
- ✅ Signal handling (SIGINT/Ctrl+C)
- ✅ ICMP Time Exceeded message handling

### Bonus Features
- ✅ **-ttl N**: Set Time To Live (1-255)
- ✅ **-c count**: Stop after sending count packets
- ✅ **-i interval**: Set delay between packets (supports decimals)

## Requirements

- Linux operating system
- GCC compiler
- Root privileges (required for raw sockets)
- Standard C libraries

## Compilation

```bash
make        # Compile the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Recompile from scratch
```

The compiled binary will be named `ping`.

## Usage

```bash
sudo ./ping [OPTIONS] destination

Options:
  -c count      Stop after sending count packets
  -i interval   Wait interval seconds between packets (default: 1.0)
  -ttl N        Set Time To Live (1-255)
```

### Basic Examples

**Ping an IP address:**
```bash
sudo ./ping 8.8.8.8
```

**Ping a hostname:**
```bash
sudo ./ping google.com
```

**Send exactly 5 packets:**
```bash
sudo ./ping -c 5 8.8.8.8
```

**Use 0.5 second interval:**
```bash
sudo ./ping -i 0.5 google.com
```

**Set custom TTL:**
```bash
sudo ./ping -ttl 64 8.8.8.8
```

**Combine multiple options:**
```bash
sudo ./ping -c 10 -i 0.2 -ttl 128 google.com
```

## Output Format

### Normal Operation
```
PING google.com (142.250.178.142): 56 data bytes
64 bytes from 142.250.178.142: icmp_seq=0 ttl=114 time=4.664 ms
64 bytes from 142.250.178.142: icmp_seq=1 ttl=114 time=8.552 ms
64 bytes from 142.250.178.142: icmp_seq=2 ttl=114 time=6.338 ms
^C
--- google.com ping statistics ---
3 packets transmitted, 3 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 4.664/6.518/8.552/1.594 ms
```

### TTL Exceeded (when TTL is too low)
```
PING 8.8.8.8 (8.8.8.8): 56 data bytes
From 10.0.1.1 icmp_seq=0 Time to live exceeded
From 10.0.1.1 icmp_seq=1 Time to live exceeded
```

### Timeout
```
PING 192.168.1.1 (192.168.1.1): 56 data bytes
Request timeout for icmp_seq 0
Request timeout for icmp_seq 1
```

## Testing

An exhaustive test suite is included with 36 comprehensive tests:

```bash
chmod +x test_ping.sh
sudo ./test_ping.sh
```

### Test Categories

1. **Basic Functionality** (4 tests)
   - IP address ping
   - Hostname resolution
   - Localhost ping
   - Statistics display

2. **Count Option** (3 tests)
   - Various packet counts
   - Automatic exit verification

3. **Interval Option** (3 tests)
   - Fast intervals (0.1s, 0.2s)
   - Slow intervals (2s)

4. **TTL Option** (4 tests)
   - TTL=1 (Time Exceeded)
   - TTL=64 (default)
   - TTL=255 (maximum)

5. **Combined Options** (4 tests)
   - Multiple options together

6. **Error Handling** (11 tests)
   - Invalid arguments
   - Missing parameters
   - Non-existent hosts

7. **Edge Cases** (3 tests)
   - Large packet counts
   - Multiple destinations

8. **Statistics Accuracy** (3 tests)
   - Packet count verification
   - Packet loss calculation
   - Format validation

## Technical Details

### Architecture

- **Socket Type**: `SOCK_RAW` with `IPPROTO_ICMP`
- **Checksum**: Manual calculation using one's complement
- **Timing**: High-precision microsecond timing with `gettimeofday()`
- **Statistics**: Real-time min/avg/max/stddev calculation

### File Structure

```
ping/
├── ping.c          # Main program logic and argument parsing
├── network.c       # Socket operations and packet handling
├── utils.c         # Checksum calculation and statistics
├── ping.h          # Header file with structures and prototypes
├── Makefile        # Build configuration
├── test_ping.sh    # Comprehensive test suite
└── README.md       # This file
```

### Key Data Structures

```c
typedef struct s_ping_pkt {
    struct icmphdr hdr;
    char msg[PACKET_SIZE - sizeof(struct icmphdr)];
} t_ping_pkt;

typedef struct s_ping_stats {
    char *hostname;
    char *ip_addr;
    unsigned int packets_sent;
    unsigned int packets_received;
    double min_rtt;
    double max_rtt;
    double sum_rtt;
    double sum_sq_rtt;
    struct timeval start_time;
    int ttl;
    int count;
    double interval;
} t_ping_stats;
```

### ICMP Packet Flow

1. **Send**: Create ICMP ECHO_REQUEST packet
   - Set type to ICMP_ECHO
   - Add timestamp to payload
   - Calculate checksum
   - Send via raw socket

2. **Receive**: Process incoming packets
   - Parse IP header
   - Extract ICMP header
   - Verify packet ID and sequence
   - Calculate RTT from timestamp
   - Handle ICMP_ECHOREPLY or ICMP_TIME_EXCEEDED

## Common Issues

### "Lacking privilege for raw socket"
Raw sockets require root privileges. Run with `sudo`:
```bash
sudo ./ping 8.8.8.8
```

### "unknown host"
The hostname cannot be resolved. Check:
- DNS configuration
- Network connectivity
- Hostname spelling

### "Request timeout"
Possible causes:
- Host is down or unreachable
- Firewall blocking ICMP
- Network issues
- TTL too low for remote host

## Statistics Calculation

### Round-Trip Time (RTT)
- **Min**: Minimum RTT observed
- **Avg**: Average of all RTTs
- **Max**: Maximum RTT observed
- **Stddev**: Standard deviation (sqrt(variance))

### Packet Loss
```
packet_loss = (sent - received) / sent × 100%
```

## Differences from Standard Ping

This implementation focuses on core ICMP functionality with bonus features. Some advanced features of standard `ping` are not implemented:

- IPv6 support
- Packet size option (-s)
- Flood mode (-f)
- Adaptive ping (-A)
- Source address specification

## License

This is an educational project for learning network programming and ICMP protocol implementation.

## Author

Developed as part of a systems programming curriculum focusing on network protocols and raw socket programming.

## References

- [RFC 792 - Internet Control Message Protocol](https://tools.ietf.org/html/rfc792)
- [RFC 1122 - Requirements for Internet Hosts](https://tools.ietf.org/html/rfc1122)
- Linux `man` pages: `ping(8)`, `icmp(7)`, `raw(7)`
