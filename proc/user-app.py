def main():
    # Seeking not available for proc files, must open for read-only and write-only
    driver_handle = open('/proc/mick_driver', 'w')
    driver_handle.write("Hello, World!!");
    driver_handle.close();
    driver_handle = open('/proc/mick_driver', 'r')
    message_from_kernel_space = driver_handle.readline()
    print(message_from_kernel_space)
    driver_handle.close();
    return

main()
