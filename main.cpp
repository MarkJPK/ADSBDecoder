#include <cstdio>

#include <libusb-1.0/libusb.h>

using namespace std;

//////////////////////////////////////
//  Procedure to display an indent  //
//////////////////////////////////////

void display_indent(int level)
{
  for(int i = 0 ; i < level ; i++)
  {
    printf("  ");
  }
}

//////////////////////////////////////////////////////
//  Procedure to display information about a device //
//////////////////////////////////////////////////////

void display_device_information(ssize_t device_index, libusb_device *device)
{
  libusb_device_descriptor  device_descriptor;
  libusb_config_descriptor *configuration_descriptor = NULL;

  const libusb_interface            *interface            = NULL;
  const libusb_interface_descriptor *alternate_setting    = NULL;
  const libusb_endpoint_descriptor  *endpoint  = NULL;

  int return_code = -1;

  //  Display the device index

  printf("\nDevice Index = %zd\n", device_index);

  //  Get the device descriptor

  if((return_code = libusb_get_device_descriptor(device, &device_descriptor)) < 0)
  {
    //  Unable to get the device descriptor

    display_indent(1);  printf("\tUnable to get the device descriptor\n");

    //  Finished

    return;
  }

  //  Display the header

  printf("\n");

  display_indent(1);  printf("Number of possible configurations : %u\n", device_descriptor.bNumConfigurations);
  display_indent(1);  printf("Device Class                      : %u\n", device_descriptor.bDeviceClass);
  display_indent(1);  printf("Vendor ID                         : %u (0x%08x)\n", device_descriptor.idVendor,  device_descriptor.idVendor);
  display_indent(1);  printf("Product ID                        : %u (0x%08x)\n", device_descriptor.idProduct, device_descriptor.idProduct);

  //  Get the configuration descriptor

  if((return_code = libusb_get_config_descriptor(device, 0, &configuration_descriptor)) < 0)
  {
    //  Unable to get the configuration descriptor

    display_indent(1);  printf("Unable to get the configuration descriptor\n");

    //  Finished

    return;
  }

  //  Display the interfaces

  display_indent(1);  printf("Number of Interfaces              : %u\n", configuration_descriptor->bNumInterfaces);

  //  Loop through each interface

  for(uint8_t interface_index = 0 ; interface_index < configuration_descriptor->bNumInterfaces ; interface_index++)
  {
    //  Get a pointer to the interface at this index

    interface = &configuration_descriptor->interface[interface_index];

    //  Display the interface

    display_indent(2);  printf("Interface Index = %u\n", interface_index);
    display_indent(2);  printf("Number of alternate settings    : %d\n", interface->num_altsetting);

    //  Loop through each alternate setting

    for(int alternate_setting_index = 0 ; alternate_setting_index < interface->num_altsetting ; alternate_setting_index++)
    {
      //  Get a pointer to the alternate setting at this index

      alternate_setting = &interface->altsetting[alternate_setting_index];

      //  Display the alternate setting

      display_indent(3);  printf("Alternate Setting Index = %u\n", alternate_setting_index);
      display_indent(3);  printf("Interface Number              : %u\n", alternate_setting->bInterfaceNumber);
      display_indent(3);  printf("Number of endpoints           : %u\n", alternate_setting->bNumEndpoints);

      //  Loop through each endpoint

      for(uint8_t endpoint_index = 0 ; endpoint_index < alternate_setting->bNumEndpoints ; endpoint_index++)
      {
        //  Get a pointer to the endpoint at this index

        endpoint = &alternate_setting->endpoint[endpoint_index];

        //  Display the endpoint

        display_indent(4);  printf("Endpoint Index = %u\n", endpoint_index);
        display_indent(4);  printf("Descriptor Type             : %u\n", endpoint->bDescriptorType);
        display_indent(4);  printf("Endpoint Address            : %u\n", endpoint->bEndpointAddress);
      }
    }
  }

  //  Free the configuration descriptor

  libusb_free_config_descriptor(configuration_descriptor);
}

//////////////////
//  Entry Point //
//////////////////

int main()
{
  libusb_device  **device_list  = NULL;
  libusb_context  *context      = NULL;

  int return_code = -1;

  ssize_t number_of_devices = -1;

  //  Initialise a library context

  if((return_code = libusb_init(&context)) < 0)
  {
    //  Unable to initialise library context

    printf("Unable to initialise a library context - Error Code : %d\n", return_code);

    //  Finished

    return(1);
  }

  //  Set debugging verbosity level to 3

  libusb_set_debug(context, 3);

  //  Get a list of connected USB devices

  if((number_of_devices = libusb_get_device_list(context, &device_list)) < 0)
  {
    //  Unable to get a list of connected USB devices

    printf("Unable to get a list of connected USB devices - Error Code : %d\n", return_code);
  }
  else
  {
    //  Display the number of connected USB devices in the list

    printf("Found %zd connected USB devices\n", number_of_devices);

    //  Loop through each connected USB device in the list

    for(ssize_t device_index = 0 ; device_index < number_of_devices ; device_index++)
    {
      //  Display information about the connected USB device

      display_device_information(device_index, device_list[device_index]);
    }

    //  Free the device list and unreference the connected USB devices in it

    libusb_free_device_list(device_list, 1);
  }

  //  Close the library context

  libusb_exit(context);

  //  Finished

  return(0);
}
