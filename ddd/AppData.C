#include "config.h"

#include "AppData.h"

#ifndef IF_MOTIF
AppData::AppData(void)
{
    // FIXME: This is just a fudge to get a working configuration
    session = strdup("");
    initial_session = strdup("");
    debugger = strdup("gdb");
    debugger_command = strdup("");
    auto_debugger = true;
    open_selection = false;
    trace = false;
    play_log = NULL;
    bash_init_commands = strdup("set prompt bashdb$_Dbg_less$_Dbg_greater$_Dbg_space\n");
    bash_settings = strdup("");
    dbg_init_commands = strdup("");
    dbg_settings = strdup("");
    dbx_init_commands = strdup("sh stty -echo -onlcr\nset $page = 1\n");
  dbx_settings = strdup(""); 
  gdb_init_commands = strdup("set prompt (gdb) \nset height 0\nset width 0\nset annotate 1\n set verbose off\n"); 
  gdb_settings = strdup("set auto-solib-add on\n"
			"set backtrace past-main off\n"
			"set backtrace past-zero-pc off\n"
			"set coerce-float-to-double on\n"
			"set confirm on\n"
			"set editing on\n"
			"set exec-done-display off\n"
			"set history expansion off\n"
			"set history save off\n"
			"set logging overwrite off\n"
			"set logging redirect off\n"
			"set opaque-type-resolution on\n"
			"set overload-resolution on\n"
			"set pagination on\n"
			"set print address on\n"
			"set print array off\n"
			"set print asm-demangle off\n"
			"set print demangle on\n"
			"set print null-stop off\n"
			"set print object off\n"
			"set print pascal_static-members on\n"
			"set print pretty on\n"
			"set print sevenbit-strings off\n"
			"set print static-members on\n"
			"set print symbol-filename off\n"
			"set print union on\n"
			"set print vtbl off\n"
			"set remotebreak off\n"
			"set remotecache off\n"
			"set step-mode off\n"
			"set symbol-reloading off\n"
			"set trust-readonly-sections off\n"
			"set unwindonsignal off\n"
			"set verbose off\n"
			"set write off\n"
			"set can-use-hw-watchpoints 1\n"
			"set debug arch 0\n"
			"set debug event 0\n"
			"set debug expression 0\n"
			"set debug frame 0\n"
			"set debug lin-lwp 0\n"
			"set debug observer 0\n"
			"set debug overload 0\n"
			"set debug remote 0\n"
			"set debug serial 0\n"
			"set debug solib 0\n"
			"set debug target 0\n"
			"set debugvarobj 0\n"
			"set stop-on-solib-events 0\n"
			"set architecture auto\n"
			"set endian auto\n"
			"set demangle-style auto\n"
			"set language auto\n"
			"set disassembly-flavor att\n"
			"set follow-fork-mode parent\n"
			"set check range auto\n"
			"set check type auto\n"
			"set annotate 1\n"
			"set backtrace limit 0\n"
			"set breakpoint pending auto\n"
			"set case-sensitive auto\n"
			"set charset `ISO-8859-1\'\n"
			"set complaints 0\n"
			"set cp-abi auto\n"
			"set debug infrun \"infrun\"\n"
			"set debug-file-directory /usr/lib/debug\n"
			"set download-write-size 0d512\n"
			"set gnutarget auto\n"
			"set height 0\n"
			"set history filename /home/prw/.gdb_history\n"
			"set history size 0d1000\n"
			"set host-charset ISO-8859-1\n"
			"set input-radix 0d10\n"
			"set listsize 0d10\n"
			"set logging file gdb.txt\n"
			"set max-user-call-depth 0d1024\n"
			"set os \n"
			"set output-radix 0d10\n"
			"set print elements 0d200\n"
			"set print max-symbolic-offset 0\n"
			"set print repeats 0d10\n"
			"set prompt (gdb) \n"
			"set remote hardware-breakpoint-limit 4294967295\n"
			"set remote hardware-watchpoint-limit 4294967295\n"
			"set remote memory-read-packet-size 0\n"
			"set remote memory-write-packet-size 0\n"
			"set remote system-call-allowed not allowed\n"
			"set remoteaddresssize 0d64\n"
			"set remotebaud 4294967295\n"
			"set remotelogbase ascii\n"
			"set remotetimeout 0d2\n"
			"set solib-search-path \n"
			"set struct-convention default\n"
			"set target-charset ISO-8859-1\n"
			"set tui active-border-mode bold-standout\n"
			"set tui border-kind acs\n"
			"set tui border-mode normal\n"
			"set watchdog 0\n"
			"set width 0\n"
			"set confirm off\n"
			"define vsl\n"
			"ddd_settings\n"
			"set args -I ./../vsllib ./ddd.vsl\n"
			"echo Enter `run\' to start VSL.\\n\n"
			"end\n"
			"define string\n"
			"p $.rep->s[0]@$.rep->len\n"
			"end\n"
			"define scmprint\n"
			"call scm_display($arg0, scm_current_output_port())\n"
			"call scm_flush_all_ports()\n"
			"call printf(\"\\n"
			"\")\n"
			"end\n"
			"define pstring\n"
			"p $arg0.rep->s[0]@$arg0.rep->len\n"
			"end\n"
			"define psecnames\n"
			"set $sect=$arg0->sections\n"
			"set $i=0\n"
			"while ($sect)\n"
			"printf \"Section %2d=%-20s (0x%08x, 0x%08x)\\n"
			"\", $i, $sect->name, $sect->_raw_size, $sect->_cooked_size\n"
			"set $i=$i+1\n"
			"set $sect=$sect->next\n"
			"end\n"
			"end\n"
			"define pscm\n"
			"call scm_display($arg0, scm_op)\n"
			"call scm_flush(scm_op)\n"
			"call printf(\"\\n"
			"\")\n"
			"end\n"
			"define perl\n"
			"ddd\n"
			"set args --perl --trace ./perltest.pl\n"
			"end\n"
			"define listsegsof\n"
			"listsegs $arg0->any_segment\n"
			"end\n"
			"define listsegs\n"
			"set $seg=$arg0\n"
			"while ($seg)\n"
			"if ($seg->type.name[0] == \'c\')\n"
			"printf \"Segment (%s): \\\"%s\\\"\\n"
			"\", $seg->type.name, $seg->body.chars\n"
			"else\n"
			"printf \"Segment (%s)\\n"
			"\", $seg->type.name\n"
			"end\n"
			"set $seg=$seg->next\n"
			"end\n"
			"end\n"
			"define fltprint\n"
			"echo \"st0=\"\n"
			"print $st0\n"
			"echo \"st1=\"\n"
			"print $st1\n"
			"echo \"st2=\"\n"
			"print $st2\n"
			"echo \"st3=\"\n"
			"print $st3\n"
			"echo \"st4=\"\n"
			"print $st4\n"
			"echo \"st5=\"\n"
			"print $st5\n"
			"echo \"st6=\"\n"
			"print $st6\n"
			"echo \"st7=\"\n"
			"print $st7\n"
			"end\n"
			"define declprint\n"
			"printf \"name=\\\"%s\\\"\\n"
			"\", $arg0->decl.name->identifier.id.str\n"
			"printf \"C type=\"\n"
			"call print_c_tree(stdout, $arg0->common.type)\n"
			"printf \"destructor_attr=%d\\n"
			"\", $arg0->decl.lang_specific->decl_flags.destructor_attr\n"
			"end\n"
			"define debug_c_tree\n"
			"call debug_c_tree($arg0)\n"
			"end\n"
			"define ddd_settings\n"
			"set verbose on\n"
			"set print pretty on\n"
			"set print object on\n"
			"set print static-members off\n"
			"set editing on\n"
			"set history expansion on\n"
			"set history filename .gdbhist\n"
			"set history save on\n"
			"set history size 1000\n"
			"directory .\n"
			"set environment DDD_HOME ./..\n"
			"set environment XUSERFILESEARCHPATH %N\n"
			"set environment XAPPLRESDIR .\n"
			"end\n"
			"define ddd\n"
			"ddd_settings\n"
			"set args --gdb --trace cxxtest\n"
			"b ddd_x_fatal\n"
			"b ddd_x_error\n"
			"b ddd_xt_error\n"
			"b ddd_fatal\n"
			"b ddd_signal\n"
			"echo Enter `run\' to start DDD.\\n\n"
			"end\n"
			"set confirm on\n");
  jdb_init_commands = strdup(""); 
  jdb_settings = strdup(""); 
  perl_init_commands = strdup("o CommandSet=580\n"); 
  perl_settings = strdup(""); 
  pydb_init_commands = strdup(""); 
  pydb_settings = strdup(""); 
  xdb_init_commands = strdup("sm\ndef run r\ndef cont c\ndef next S\ndef step s\ndef quit q\ndef finish { bu \\1t ; c ; L }\n"); 
  xdb_settings = strdup(""); 
  restart_commands = strdup(""); 
  source_init_commands = true;
  init_symbols = strdup("main\nMAIN\nmain_\nMAIN_\nmain__\nMAIN__\n_main\n_MAIN\n__main\n__MAIN\nmy_main\nthe_main\nvxworks_main\nmain_vxworks"); 
  vsl_path = strdup("user_themes:ddd_themes:."); 
  vsl_library = strdup("builtin"); 
  vsl_defs = strdup(""); 
  vsl_base_defs = strdup(""); 
  default_font = strdup("helvetica-bold"); 
  variable_width_font = strdup("helvetica-bold"); 
  fixed_width_font = strdup("lucidatypewriter-bold"); 
  data_font = strdup("courier-bold"); 
  default_font_size = 140, 
  variable_width_font_size = 140, 
  fixed_width_font_size = 180, 
  data_font_size = 180, 
  font_select_command = strdup("xfontsel -bg 'grey' -fg 'black' -fn '@FONT@' -title 'DDD: @TYPE@ selector' -print"); 
  tab_width = 8, 
  lines_above_cursor = 2, 
  lines_below_cursor = 3, 
  indent_source = 0, 
  indent_script = 4, 
  indent_code = 4, 
  line_number_width = 5, 
      source_editing = true; 
  line_buffered_console = true;
  console_has_focus = On;
  bash_display_shortcuts = strdup(""); 
  dbg_display_shortcuts = strdup(""); 
  dbx_display_shortcuts = strdup(""); 
  gdb_display_shortcuts = strdup("/t ()\t// Convert to Bin\n/d ()\t// Convert to Dec\n/x ()\t// Convert to Hex\n/o ()\t// Convert to Oct"); 
  jdb_display_shortcuts = strdup(""); 
  perl_display_shortcuts = strdup("sprintf(\"%#x\", ())\t// Convert to Hex\nsprintf(\"%#o\", ())\t// Convert to Oct"); 
  pydb_display_shortcuts = strdup("/t ()\t// Convert to Bin\n/d ()\t// Convert to Dec\n/x ()\t// Convert to Hex\n/o ()\t// Convert to Oct"); 
  xdb_display_shortcuts = strdup(""); 
  console_buttons = strdup("scmprint ()\nfltprint\ndeclprint ()\ndebug_c_tree ()\nlistsegs ()\nlistsegsof ()"); 
  source_buttons = strdup(""); 
  data_buttons = strdup(""); 
  tool_buttons = strdup("run\nbreak^C\nstep\nstepi\nnext\nnexti\nuntil\nfinish\ncont\nkill\nup\ndown\nUndo\nRedo\nEdit\nMake"); 
  label_delimiter = strdup("//"); 
  command_toolbar = false; 
  common_toolbar = true; 
  separate_data_window = false; 
  separate_source_window = false; 
  separate_exec_window = true; 
  data_window = true; 
  source_window = true; 
  debugger_console = true; 
  auto_close_data_window = true; 
  plot_command = strdup("gnuplot -bg 'grey96' -font '@FONT@' -name '@NAME@' -geometry +5000+5000"); 
  plot_term_type = strdup("xlib"); 
  plot_window_class = strdup("Gnuplot"); 
  plot_window_delay = 2000, 
  plot_init_commands = strdup("set parametric\nset urange [0:1]\nset vrange [0:1]\nset trange [0:1]\n"); 
  plot_2d_settings = strdup("set noborder"); 
  plot_3d_settings = strdup("set border"); 
  term_command = strdup("xterm -bg 'grey96' -fg 'black' -cr 'DarkGreen' -fn '@FONT@' -title 'DDD: Execution Window' -e /bin/sh -c"); 
  term_type = strdup("xterm"); 
  use_tty_command = false; 
  question_timeout = 10, 
  position_timeout = 500, 
  display_timeout = 2000, 
  synchronous_gdb = false; 
  terminate_on_eof = false; 
  stop_and_continue = true; 
  run_interrupt_delay = 2000, 
  cont_interrupt_delay = 200, 
  poll_child_status = true; 
  debugger_host = strdup(""); 
  debugger_rhost = strdup(""); 
  debugger_host_login = strdup(""); 
  rhost_init_commands = strdup("stty -echo -onlcr"); 
  rsh_command = strdup("/usr/bin/rsh"); 
  filter_files = true; 
  list_exec_command = strdup("file @MASK@ | grep '.*:.*exec.*' | grep -v  '.*:.*script.*' \t| cut -d: -f1 | grep -v '.*\\.o$'"); 
  list_source_command = strdup("file @MASK@ | grep '.*:.*text.*' | cut -d: -f1"); 
  list_core_command = strdup("file @MASK@ | grep '.*:.*core.*' | cut -d: -f1"); 
  list_dir_command = strdup("file @MASK@ | grep '.*:.*directory.*' | cut -d: -f1"); 
  uncompress_command = strdup("gzip -d -c"); 
  ps_command = strdup("ps x 2> /dev/null || ps -ef 2> /dev/null || ps"); 
  www_page = strdup("http://www.gnu.org/software/ddd/"); 
  www_command = strdup("mozilla -remote 'openURL(@URL@)' || netscape -remote 'openURL(@URL@)' || ${WWWBROWSER-false} '@URL@' || galeon 'openURL(@URL@)' || skipstone 'openURL(@URL@)' || light 'openURL(@URL@)' || mozilla '@URL@' || konqueror 'openURL(@URL@)' || kfmbrowser '@URL@' || netscape '@URL@' || gnudoit '(w3-fetch \"@URL@\")' || mosaic '@URL@' || Mosaic '@URL@' || xterm -e lynx '@URL@'");
  show_invocation = false; 
  show_version = false; 
  show_configuration = false; 
  show_manual = false; 
  show_license = false; 
  show_news = false; 
  show_fonts = false; 
  check_configuration = false; 
  print_command = strdup("lp -c"); 
  paper_size = strdup("8.5in x 11in"); 
  edit_command = strdup("${XEDITOR-false} +@LINE@ @FILE@ || xterm -e ${EDITOR-vi} +@LINE@ @FILE@"); 
  get_core_command = strdup("gcore -o @FILE@ @PID@"); 
  panned_graph_editor = false; 
  find_words_only = true; 
  find_case_sensitive = true; 
  group_iconify = false; 
  uniconify_when_ready = true; 
  transient_dialogs = true; 
  global_tab_completion = true; 
  save_history_on_exit = true; 
  save_options_on_exit = true; 
  cache_source_files = true; 
  cache_machine_code = true; 
  suppress_warnings = false; 
  warn_if_locked = false; 
  check_options = 30, 
  tty_mode = false; 
  block_tty_input = Auto, 
  buffer_gdb_output = Auto, 
  annotate = 0, 
  display_glyphs = true; 
  max_glyphs = 10, 
  cache_glyph_images = true; 
  glyph_update_delay = 5, 
  display_line_numbers = false; 
  disassemble = false; 
  max_disassemble = 256, 
  use_source_path = false; 
  all_registers = false; 
  button_tips = true; 
  value_tips = true; 
  button_docs = true; 
  value_docs = true; 
  startup_tips = false; 
  startup_tip_count = 6, 
  status_at_bottom = true; 
  toolbars_at_bottom = false; 
  max_display_title_length = 20, 
  max_popup_expr_length = 20, 
  max_value_tip_length = 60, 
  max_value_doc_length = 80, 
  button_tip_delay = 750, 
  value_tip_delay = 750, 
  button_doc_delay = 100, 
  value_doc_delay = 100, 
  clear_doc_delay = 1000, 
  clear_tip_delay = 50, 
  blink_while_busy = true; 
  busy_blink_rate = 500, 
  check_grabs = true; 
  check_grab_delay = 5000, 
  grab_action_delay = 10000, 
  grab_action = strdup("cont"); 
  detect_aliases = true; 
  typed_aliases = true; 
  delete_alias_displays = true; 
  align_2d_arrays = true; 
  array_orientation = true; 
  struct_orientation = true; 
  show_member_names = true; 
  expand_repeated_values = false; 
  bump_displays = true; 
  hide_inactive_displays = true; 
  show_base_display_titles = true; 
  show_dependent_display_titles = false; 
  cluster_displays = false; 
  display_placement = true; 
  status_history_size = 20, 
  popdown_history_size = 0, 
  sort_popdown_history = true; 
  verify_buttons = true; 
  auto_raise_tool = true; 
  decorate_tool = Auto, 
  sticky_tool = true; 
  tool_right_offset = -169, 
  tool_top_offset = -89, 
  button_captions = true; 
  button_images = true; 
  button_caption_geometry = strdup("29x7+0-0"); 
  button_image_geometry = strdup("21x17+4+0"); 
  flat_toolbar_buttons = true; 
  flat_dialog_buttons = true; 
  button_color_key = strdup("g"); 
  active_button_color_key = strdup("c"); 
  splash_screen_color_key = strdup("c"); 
  auto_commands = true; 
  auto_command_prefix = strdup(""); 
  max_breakpoint_number = 99, 
  max_display_number = 99, 
  splash_screen = true; 
  color_wm_icons = true; 
  cut_copy_paste_bindings = MotifBindings, 
  select_all_bindings = KDEBindings, 
  max_undo_depth = -1, 
  max_undo_size = 2000000, 
  themes = strdup("! green.vsl\t*\n! red.vsl\t*\nrednil.vsl\t*\n! smalltitles.vsl\t*\n! smallvalues.vsl\t*\nsuppress.vsl\t*->next;*->prev\n! tinyvalues.vsl\t*\n"); 
  suppress_theme = strdup("suppress.vsl"); 
  maintenance = false; 
  dump_core = true; 
  debug_core_dumps = false; 
  lesstif_version = 1000, 
  dddinit_version = strdup("3.3.11"); 
  app_defaults_version = strdup("3.3.11");
}
#endif // IF_MOTIF

