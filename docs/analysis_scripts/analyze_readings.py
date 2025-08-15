import re
import statistics

def parse_metrics(file_path):
    # Patterns for matching values
    rtt_pattern = re.compile(r"\[RTT\] Round-Trip Time: (\d+) us")
    decode_pattern = re.compile(r"\[TIMING\] CoAP RX->Decode: (\d+) us")
    net_delay_pattern = re.compile(r"\[TIMING\] Network Delay: (\d+) us \(([\d\.]+) ms\)")

    rtts = []
    decodes = []
    net_delays = []

    with open(file_path, "r") as f:
        for line in f:
            if match := rtt_pattern.search(line):
                rtts.append(int(match.group(1)) / 1000.0)  # convert us → ms
            elif match := decode_pattern.search(line):
                decodes.append(int(match.group(1)) / 1000.0)  # convert us → ms
            elif match := net_delay_pattern.search(line):
                net_delays.append(float(match.group(2)))

    return rtts, decodes, net_delays

def summarize_data(values, name):
    if not values:
        return f"{name}: No data"
    return (f"{name} → "
            f"Avg: {statistics.mean(values):.2f} ms, "
            f"Min: {min(values):.2f} ms, "
            f"Max: {max(values):.2f} ms, "
            f"Std Dev: {statistics.stdev(values):.2f} ms, "
            f"Count: {len(values)}")

if __name__ == "__main__":
    file_path = "/Users/Gautam/Desktop/NEU/Sem 4/Project/data_0ms.txt"  # change for each delay scenario
    rtts, decodes, net_delays = parse_metrics(file_path)

    print(summarize_data(rtts, "RTT"))
    print(summarize_data(decodes, "CoAP RX→Decode"))
    print(summarize_data(net_delays, "Network Delay"))
