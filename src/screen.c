/**
 * #FujiNet CONFIG FOR #adam
 *
 * Screen Routines
 */

#include "screen.h"
#include "globals.h"
#include "bar.h"
#include <msx.h>
#include <smartkeys.h>
#include <conio.h>
#include <sys/ioctl.h>
#include <eos.h>

static char udg[64] =
  {
   0,0,0,0,0,0,3,51,                               // WIFI 1
   0,0,3,3,51,51,51,51,                            // WIFI 2
   48,48,48,48,48,48,48,48,                        // WIFI 3
   0,120,135,255,255,255,255,0,                    // FOLDER
   0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, // Password smudge
   0xFF,0x81,0x81,0xA5,0x81,0x81,0xFF,0x00,        // DDP
   0xFF,0x81,0x8D,0x8D,0xAD,0x81,0xFF,0x00,        // DSK
   0xFF,0x81,0xBD,0xBD,0xBD,0x81,0xFF,0x00         // ROM
  };

void screen_init(void)
{
  void *param = &udg;
  console_ioctl(IOCTL_GENCON_SET_UDGS,&param);
  smartkeys_set_mode();
  smartkeys_display(NULL,NULL,NULL,NULL,NULL,NULL);
  smartkeys_status("  WELCOME TO #FUJINET");
  eos_start_read_keyboard();
}

void screen_error(const char *c)
{
  smartkeys_display(NULL,NULL,NULL,NULL,NULL,NULL);
  smartkeys_status(c);
}

void screen_set_wifi(AdapterConfig *ac)
{
  smartkeys_set_mode();
  msx_vfill(MODE2_ATTR,0xF5,0x100);
  msx_vfill(MODE2_ATTR+0x100,0x1F,0x1100);
  msx_vfill_v(MODE2_ATTR,0xF5,144);
  msx_vfill_v(MODE2_ATTR+8,0xF5,144);
  msx_vfill_v(MODE2_ATTR+16,0xF5,144);
  msx_color(15,5,7); gotoxy(8,0); cprintf("MAC: %02X:%02X:%02X:%02X:%02X:%02X",ac->macAddress[0],ac->macAddress[1],ac->macAddress[2],ac->macAddress[3],ac->macAddress[4],ac->macAddress[5]);

  smartkeys_display(NULL,NULL,NULL,NULL,NULL,"   SKIP");
  smartkeys_status("  SCANNING FOR NETWORKS...");
}

void screen_set_wifi_display_ssid(char n, SSIDInfo *s)
{
  char meter[4]={0x20,0x20,0x20,0x00};
  char ds[32];

  memset(ds,0x20,28);
  strncpy(ds,s->ssid,28);
  
  if (s->rssi > -40)
    {
      meter[0] = 0x80;
      meter[1] = 0x81;
      meter[2] = 0x82;
    }
  else if (s->rssi > -60)
    {
      meter[0] = 0x80;
      meter[1] = 0x81;
    }
  else
    {
      meter[0] = 0x80;
    }
  
  msx_color(15,5,7);
  gotoxy(0,n+1); cprintf("%s",meter);
  msx_color(1,15,7);
  cprintf("%s",ds);
}

void screen_set_wifi_select_network(unsigned char nn)
{
  smartkeys_display(NULL,NULL,NULL," HIDDEN\n  SSID"," RESCAN","  SKIP");
  sprintf(response,"  WELCOME TO #FUJINET!\n  %u NETWORKS FOUND\n  SELECT A NETWORK.",nn);
  smartkeys_status(response);
}

void screen_set_wifi_custom(void)
{
  smartkeys_display(NULL,NULL,NULL,NULL,NULL,NULL);
  smartkeys_status("  ENTER NAME OF HIDDEN NETWORK");
}

void screen_set_wifi_password(void)
{
  smartkeys_display(NULL,NULL,NULL,NULL,NULL,NULL);
  smartkeys_status("  ENTER NETWORK PASSWORD\n  AND PRESS [RETURN]");
}

void screen_connect_wifi(NetConfig *nc)
{
  smartkeys_set_mode();
  
  smartkeys_display(NULL,NULL,NULL,NULL,NULL,NULL);
  sprintf(response,"  CONNECTING TO NETWORK\n  %s",nc->ssid);
  smartkeys_status(response);
}

char* screen_hosts_and_devices_slot(char *c)
{
  return c[0]==0x00 ? "Empty" : c;
}

void screen_hosts_and_devices_device_slots(unsigned char y, DeviceSlot *d)
{
  for (char i=0;i<4;i++)
    {
      gotoxy(0,i+y); cprintf("%d%s",i+1,screen_hosts_and_devices_slot(d[i].file));
    }
}

void screen_hosts_and_devices(HostSlot *h, DeviceSlot *d)
{
  smartkeys_set_mode();
  eos_start_read_keyboard();
  gotoxy(0,0);  cprintf("%32s","HOST SLOTS");
  gotoxy(0,11); cprintf("%32s","DISK SLOTS");
  
  for (char i=0;i<8;i++)
    {
      gotoxy(0,i+1); cprintf("%d%s",i+1,screen_hosts_and_devices_slot(h[i])); 
    }

  screen_hosts_and_devices_device_slots(12,d);
  
  msx_vfill(MODE2_ATTR,0xF4,256);
  msx_vfill(MODE2_ATTR+0x0B00,0xF4,256);
  msx_vfill(MODE2_ATTR+0x0100,0x1F,2048);
  msx_vfill(MODE2_ATTR+0x0C00,0x1F,1024);
  msx_vfill_v(MODE2_ATTR+0x0100,0xF4,64);
  msx_vfill_v(MODE2_ATTR+0x0C00,0xF4,32);
}

void screen_hosts_and_devices_hosts(void)
{
  smartkeys_display(NULL,NULL,NULL,"  SHOW\n CONFIG","  EDIT\n  SLOT","  BOOT");
  smartkeys_status("  [RETURN] SELECT HOST\n  [1-8] SELECT SLOT\n  [TAB] GO TO DISK SLOTS");
  bar_set(0,1,8,0);
}

void screen_hosts_and_devices_devices(void)
{
}

void screen_hosts_and_devices_clear_host_slot(unsigned char i)
{
  gotoxy(1,i+1);
  msx_vfill((i+1)*0x0100+8,0x00,248);
}

void screen_hosts_and_devices_edit_host_slot(unsigned char i)
{
  smartkeys_display(NULL,NULL,NULL,NULL,NULL,NULL);
  sprintf(response,"  EDIT THE HOST NAME FOR SLOT %u\n  PRESS [RETURN] WHEN DONE.",i);
  smartkeys_status(response);
}

void screen_show_info(AdapterConfig* ac)
{
  smartkeys_set_mode();

  gotoxy(0,7);
  
  cprintf("%32s","SSID");
  cprintf("%32s",ac->ssid);
  cprintf("%10s%s\n","HOSTNAME:",ac->hostname);
  cprintf("%10s%u.%u.%u.%u\n","IP:",ac->localIP[0],ac->localIP[1],ac->localIP[2],ac->localIP[3]);
  cprintf("%10s%u.%u.%u.%u\n","NETMASK:",ac->netmask[0],ac->netmask[1],ac->netmask[2],ac->netmask[3]);
  cprintf("%10s%u.%u.%u.%u\n","DNS:",ac->dnsIP[0],ac->dnsIP[1],ac->dnsIP[2],ac->dnsIP[3]);
  cprintf("%10s%02x:%02x:%02x:%02x:%02x:%02x\n","MAC:",ac->macAddress[0],ac->macAddress[1],ac->macAddress[2],ac->macAddress[3],ac->macAddress[4],ac->macAddress[5]);
  cprintf("%10s%02x:%02x:%02x:%02x:%02x:%02x\n","BSSID:",ac->bssid[0],ac->bssid[1],ac->bssid[2],ac->bssid[3],ac->bssid[4],ac->bssid[5]);
  cprintf("%10s%s\n","FNVER:",ac->fn_version);

  msx_vfill(MODE2_ATTR+0x0700,0xF4,256);
  msx_vfill(MODE2_ATTR+0x0800,0x1F,256);
  
  for (char i=0;i<7;i++)
    {
      msx_vfill(MODE2_ATTR+(i*256)+0x900,0xF4,80);
      msx_vfill(MODE2_ATTR+(i*256)+0x900+80,0x1F,176);
    }
    
  smartkeys_display(NULL,NULL,NULL,NULL," CHANGE\n  SSID","RECONNECT");
  smartkeys_status("  FUJINET CONFIGURATION\n  PRESS ANY KEY\n  TO RETURN TO HOSTS");
}

void screen_select_file(void)
{
  smartkeys_set_mode();
  msx_color(15,4,7);
  msx_vfill(MODE2_ATTR,0xF4,512);

  // Paint content area
  msx_vfill(MODE2_ATTR+0x200,0x1F,0x1100);
  smartkeys_display(NULL,NULL,NULL,NULL,NULL,NULL);
  smartkeys_status("  OPENING...");
}

void screen_select_file_display(char *p, char *f)
{
  // Clear content area
  msx_vfill(0x0000,0x00,0x1400);

  // Update content area
  msx_color(15,4,7);
  gotoxy(0,0); cprintf("%32s",selected_host_name);

  if (f[0]==0x00)
    cprintf("%32s",p);
  else
    cprintf("%24s|%8s|",p,f);

}

void screen_select_file_prev(void)
{
  msx_color(1,15,7);
  gotoxy(1,0); cprintf("%32s","[...]");
}

void screen_selecct_file_next(void)
{
  msx_color(1,15,7);
  gotoxy(17,0); cprintf("%32s","[...]");
}

void screen_select_file_display_entry(unsigned char y, char* e)
{
  msx_color(1,15,7);
  gotoxy(0,y+3);
  cprintf("%-31s",e);
}

void screen_select_file_choose(char visibleEntries)
{
  bar_set(2,1,visibleEntries,0); // TODO: Handle previous
  smartkeys_display(NULL,NULL,NULL,"  UP","FILTER","  BOOT");
}

void screen_select_slot()
{
}