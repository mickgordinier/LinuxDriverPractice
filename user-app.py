def main():
    driver_handle = open('/proc/mick_driver')
    message_from_kernel_space = driver_handle.readline()
    print(message_from_kernel_space)
    driver_handle.close();
    return

main()
