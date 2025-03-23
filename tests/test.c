#include <GLFW/glfw3.h>
#include <leif/asset_manager.h>
#include <leif/color.h>
#include <leif/ez_api.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
#include <leif/ui_core.h>
#include <leif/util.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/widgets/text.h>
#include <leif/win.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <wctype.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
  lf_ui_state_t* ui;
  lf_widget_t* div;
} state_t;

static state_t s;

float val = 0;

static void comp();


void run_command_silent(const char *cmd) {
    pid_t pid = fork();
    
    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        return;
    }
    
    if (pid == 0) {
        // In child process
        
        // Redirect stdin, stdout, and stderr to /dev/null
        int devnull = open("/dev/null", O_RDWR);
        if (devnull == -1) {
            perror("open /dev/null failed");
            exit(1);
        }
        
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);
        
        // Execute the command using /bin/sh
        execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        
        // If execl fails
        exit(1);
    }

    // Parent process does not wait for child
}
void on_slide(lf_ui_state_t* ui, lf_widget_t* widget, float* val) {
  char buf[32];
  sprintf(buf, "amixer sset Master %f% & ", *val);
  run_command_silent(buf);
  lf_component_rerender(s.ui, comp);
}

void comp() {
  lf_text_h4(s.ui, "devicons:                                                                                                                                                                                                      ");

  lf_text_h4(s.ui, "weather_icons:                                                                                                                                                                                                                                     摒 敖 晴 朗 望 杖 歹 殺 流 滛 滋 漢 瀞 煮 瞧");
  char buf[32];
  sprintf(buf, "%.2f", val);

  lf_button(s.ui);
  lf_style_widget_prop(s.ui, lf_crnt(s.ui), corner_radius_percent, 50.0f);
  lf_text_h4(s.ui, buf);
  lf_button_end(s.ui);

  lf_slider(s.ui, &val, 0, 100);
  ((lf_slider_t*)lf_crnt(s.ui))->on_slide = on_slide;
  lf_crnt(s.ui)->container.size.x = 300;
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  printf("Win: %i\n", (int)win);
  
  lf_window_t win2 = lf_ui_core_create_window(1280, 720, "hello leif");
  printf("Win2: %i\n", (int)win2);
    s.ui = lf_ui_core_init(win);

  lf_ui_state_t* ui2 = lf_ui_core_init(win2);

  lf_widget_set_font_family(s.ui, s.ui->root, "JetBrainsMono Nerd Font");
  lf_widget_set_font_style(s.ui, s.ui->root, LF_FONT_STYLE_BOLD);
lf_widget_set_fixed_height_percent(s.ui, lf_crnt(s.ui), 100.0f);
lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical | AlignCenterHorizontal);


  lf_button(s.ui);
  lf_style_widget_prop(s.ui, lf_crnt(s.ui), corner_radius_percent, 50.0f);
  lf_text_h4(s.ui, "Hello, World!");
  lf_button_end(s.ui);


  lf_widget_set_font_family(ui2, ui2->root, "JetBrainsMono Nerd Font");
  lf_widget_set_font_style(ui2, ui2->root, LF_FONT_STYLE_BOLD);
  lf_widget_set_fixed_height_percent(ui2, lf_crnt(ui2), 100.0f);
  lf_widget_set_alignment(lf_crnt(ui2), AlignCenterVertical | AlignCenterHorizontal);


  lf_text_h4(ui2, "Bye, World!");
   
  while(s.ui->running ||  ui2->running) {
    lf_ui_core_next_event(s.ui);
    lf_ui_core_next_event(ui2);
  }
  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
