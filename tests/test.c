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
  char buf[32];
  sprintf(buf, "%.2f", val);
  lf_text_h4(s.ui, buf);
  lf_slider(s.ui, &val, 0, 100);
  ((lf_slider_t*)lf_crnt(s.ui))->on_slide = on_slide;
  lf_crnt(s.ui)->container.size.x = 300;
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
    s.ui = lf_ui_core_init(win);


  s.ui = lf_ui_core_init(win);

  lf_widget_set_font_family(s.ui, s.ui->root, "JetBrainsMono Nerd Font");
  lf_widget_set_font_style(s.ui, s.ui->root, LF_FONT_STYLE_BOLD);
lf_widget_set_fixed_height_percent(s.ui, lf_crnt(s.ui), 100.0f);
lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical | AlignCenterHorizontal);

     FILE *fp;
    char buffer[128];
    
    // Run the pactl command and read its output
    fp = popen("pactl get-sink-volume @DEFAULT_SINK@ | awk '{print $5}' | tr -d '%'", "r");
    if (fp == NULL) {
        printf("Failed to get volume\n");
        return 1;
    }

    // Read the output (volume percentage)
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        val = atoi(buffer);
    }

    pclose(fp);
  lf_component(s.ui, comp);

   
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
