#! /usr/bin/env python

# pydb.py -- a Python debugger!
# Modified original pdb.py

# (See pdb.doc for original documentation.)

import string
import sys
import linecache
import cmd
import bdb
import repr
import os
import posix


import pydbcmd
from pydbsupt import Breakpoint, effective, Display, DisplayNode

# Interaction prompt line will separate file and call info from code
# text using value of line_prefix string.  A newline and arrow may
# be to your liking.  You can set it once pydb is imported using the
# command "pydb.line_prefix = '\n% '".
# line_prefix = ': '    # Use this to get the old situation back
line_prefix = '\n-> '    # Probably a better default

class Pdb(bdb.Bdb, pydbcmd.Cmd):
    
    def __init__(self):
        bdb.Bdb.__init__(self)
        pydbcmd.Cmd.__init__(self)
        self.prompt = '(Pdb) '
        self.running = 0
        self.filetodebug = ""
        self.moduletodebug = ""
        self.nextbpnumber = 1
        self.latestbp = None     # for stack print routine
        self.cmdqueue = []
    
    def reset(self):
        bdb.Bdb.reset(self)
        self.forget()
    
    def forget(self):
        self.lineno = None
        self.stack = []
        self.curindex = 0
        self.curframe = None
    
    def setup(self, f, t):
        self.forget()
        self.stack, self.curindex = self.get_stack(f, t)
        self.curframe = self.stack[self.curindex][0]
    
    # Override Bdb methods (except user_call, for now)
    
    def user_line(self, frame):
        # This function is called when we stop or break at this line
        self.interaction(frame, None)
    
    def user_return(self, frame, return_value):
        # This function is called when a return trap is set here
        frame.f_locals['__return__'] = return_value
        print '--Return--'
        self.interaction(frame, None)
    
    def user_exception(self, frame, (exc_type, exc_value, exc_traceback)):
        # This function is called if an exception occurs,
        # but only if we are to stop at or just below this level
        frame.f_locals['__exception__'] = exc_type, exc_value
        if type(exc_type) == type(''):
            exc_type_name = exc_type
        else: exc_type_name = exc_type.__name__
        print exc_type_name + ':', repr.repr(exc_value)
        self.interaction(frame, exc_traceback)
    
    # General interaction function
    
    def interaction(self, frame, traceback):
        if self.running != 0:
            self.setup(frame, traceback)
            self.print_stack_entry(self.stack[self.curindex])
            self.display.displayAny(self.curframe)
            try:
                self.cmdloop()
            except KeyboardInterrupt:
                self.do_kill(None)
        self.latestbp = None    # Can forget it now that we're moving on
        self.forget()

    def default(self, line):
        if line[:1] == '!': line = line[1:]
        # Try both the local and "outer" environments.
        both = 0
        try:
            locals = self.curframe.f_locals
            globals = self.curframe.f_globals
            both = 1
        except:
            locals = self.outerlocals
            globals = self.outerglobals
        globals['__privileged__'] = 1
        try:
            code = compile(line + '\n', '<stdin>', 'single')
            exec code in globals, locals
        except:
            if both:
                # Failed with current frame, so try 'outer' world.
                locals = self.outerlocals
                globals = self.outerglobals
                globals['__privileged__'] = 1
                try:
                    code = compile(line + '\n', '<stdin>', 'single')
                    exec code in globals, locals
                except:
                    t, v = sys.exc_info()[:2]
                    if type(t) == type(''):
                        exc_type_name = t
                    else: exc_type_name = t.__name__
                    print '***', exc_type_name + ':', v
            else:
                t, v = sys.exc_info()[:2]
                if type(t) == type(''):
                    exc_type_name = t
                else: exc_type_name = t.__name__
                print '***', exc_type_name + ':', v

    # Override Bdb (note number of args is different)
    def set_break(self, filename, lineno, temporary=0):
        import linecache # Import as late as possible
        line = linecache.getline(filename, lineno)
        if not line:
            return 'That line does not exist!'
        if not self.breaks.has_key(filename):
            self.breaks[filename] = []
        list = self.breaks[filename]
        if not lineno in list:
            list.append(lineno)
        bp = Breakpoint(filename, lineno, temporary)
        print 'Breakpoint %d, at %s:%d.' %(bp.number, filename, lineno)
    
    # Override Bdb
    def break_here(self, frame):
        filename=frame.f_code.co_filename
        if not self.breaks.has_key(filename):
            return 0
        lineno=frame.f_lineno
        if not lineno in self.breaks[filename]:
            return 0
        # flag says ok to delete temp. bp
        (bp, flag) = effective(filename, lineno, frame)
        if bp:
            self.latestbp = bp.number
            if (flag and bp.temporary):
                self.do_delete(str(bp.number))
            return 1
        else:
            return 0

    # Command definitions, called by cmdloop()
    # The argument is the remaining string on the command line
    # Return true to exit from the command loop 
    
    do_h = pydbcmd.Cmd.do_help

    def do_break(self, arg, temporary = 0):
        if not arg:
            print self.get_all_breaks()
            return
        f_l = string.split(arg, ':')
        # Setting a breakpoint at func:line
        if len(f_l) == 2:
            filename = f_l[0]
            lineno = int(f_l[1])
            line = self.checkline(filename, lineno)
            if line:
                # now set the break point
                err = self.set_break(filename, lineno, temporary)
                if err: print '***', err
            return

        try:    
            lineno = int(arg)
            try:
                filename = self.curframe.f_code.co_filename
            except:
                filename = self.filetodebug # not tracing yet
        except:
            # Try function name as the argument
            try:
                func = arg
                if hasattr(func, 'im_func'):
                    func = func.im_func
                code = func.func_code
                lineno = code.co_firstlineno
                filename = code.co_filename
            except:
                # One more thing to try:
                answer = self.lineinfo("'" + arg + "'")
                if not answer[0]:
                    print '*** The specified object "', arg,
                    print '" is not a function'
                    return
                (junk, filename, l) = answer
                lineno = int(l)
        line = self.checkline(filename, lineno)
        if line:
            # now set the break point
            err = self.set_break(filename, line, temporary)
            if err: print '***', err
        return

    do_b = do_break
    
    def do_tbreak(self, arg):
        self.do_break(arg, 1)

    def checkline(self, filename, lineno):
        """Return line number of first line at or after input argument
        such that if the input points to a 'def', the returned line number
        is the first non-blank/non-comment line to follow.  If the input
        points to a blank or comment line, return 0.  At end of file,
        also return 0."""

        line = linecache.getline(filename, lineno)
        if not line:
            print 'End of file'
            return 0
        line = string.strip(line)
        # Don't allow setting breakpoint at a blank line
        if ( not line or (line[0] == '#') or
         (line[:3] == '"""') or line[:3] == "'''" ):
            print '*** Blank or comment'
            return 0
        # When a file is read in and a breakpoint is at
        # the 'def' statement, the system stops there at
        # code parse time.  We don't want that, so all breakpoints
        # set at 'def' statements are moved one line onward
        if line[:3] == 'def':
            incomment = ''
            while 1:
                lineno = lineno+1
                line = linecache.getline(filename, lineno)
                if not line:
                    print 'end of file'
                    return 0
                line = string.strip(line)
                if incomment:
                    if len(line) < 3: continue
                    if (line[-3:] == incomment):
                        incomment = ''
                    continue
                if not line: continue	# Blank line
                if len(line) >= 3:
                    if ( line[:3] == '"""' or line[:3] == "'''" ):
                        incomment = line[:3]
                        continue
                if line[0] != '#': break
        return lineno

    def do_enable(self, arg):
        args = string.split(arg)
        if args[0] == 'display':
            self.display_enable(args[1:], 1)
            return
        for i in args:
            bp = Breakpoint.bpbynumber[int(i)]
            if bp:
                bp.enable()

    def do_disable(self, arg):
        args = string.split(arg)
        if args[0] == 'display':
            self.display_enable(args[1:], 0)
            return
        for i in args:
            bp = Breakpoint.bpbynumber[int(i)]
            if bp:
                bp.disable()

    def display_enable(self, arg, flag):
        # arg is list of display points
        for i in arg:
            self.display.enable(int(i), flag)

    def do_condition(self, arg):
        # arg is breakpoint number and condition
        args = string.split(arg, ' ', 1)
        bpnum = int(string.strip(args[0]))
        try:
            cond = args[1]
        except:
            cond = None
        bp = Breakpoint.bpbynumber[bpnum]
        if bp:
            bp.cond = cond
            if not cond:
                print 'Breakpoint %d is now unconditional.' % (bpnum,)

    def do_ignore(self,arg):
        # arg is bp number followed by ignore count
        args = string.split(arg)
        bpnum = int(string.strip(args[0]))
        try:
            count = int(string.strip(args[1]))
        except:
            count = 0
        bp = Breakpoint.bpbynumber[bpnum]
        if bp:
            bp.ignore = count
            if (count > 0):
                reply = 'Will ignore next '
                if (count > 1):
                    reply = reply + '%d crossings' % (count,)
                else:
                    reply = reply + '1 crossing'
                print reply + ' of breakpoint %d.' % (bpnum,)
            else:
                print 'Will stop next time breakpoint %d is reached.' % (bpnum,)

    def do_delete(self, arg):
        # Delete from a list, or delete all if no arg
        numberlist = string.split(arg)
        if numberlist:
            for i in numberlist:
                number = int(i)
                bp = Breakpoint.bpbynumber[number]
                filename = bp.file
                lineno = bp.line
                if not self.breaks.has_key(filename):
                    return 'There are no breakpoints in that file!'
                if lineno not in self.breaks[filename]:
                    return 'There is no breakpoint there!'
                # If there's only one bp in the list for that file,line
                # pair, then remove the breaks entry
                if len(Breakpoint.bplist[filename, lineno]) == 1:
                    self.breaks[filename].remove(lineno)
                if not self.breaks[filename]:
                    del self.breaks[filename]
                bp.deleteMe()
                print 'Deleted breakpoint %d ' % (number,)
        else:  # delete them all
            for bp in Breakpoint.bpbynumber:
                if bp:
                    bp.deleteMe()
            self.breaks = {}
    
    def do_where(self, arg):
        if arg:
            count = int(string.strip(arg))
            # neg count possible ?? XXX
        else:
            count = 10000   # 'huge' number
        # Last two frames are internal to debugger and don't
        # get printed
        for j in range(len(self.stack)-1, 1, -1):
            self.frame_print(j)
            count = count -1
            if count == 0:
                break
    do_w = do_where

    def frame_print(self, j):
        prefix = '#%d ' % (len(self.stack) -1 - j,)
        res = self.format_stack_entry(self.stack[j], prefix)
        print string.split(res, '\n', 1)[0]

    def do_frame(self, arg):
        if not arg:
            self.frame_print(self.curindex)
        else:
            goto = int(string.strip(arg))
            # Should never happen, but if you want to be sure:
            # Replace -1 with -3 if limited to frames of debugged program
            if (goto < 0 or goto > len(self.stack)-1):
                return
            self.curindex = len(self.stack) - 1 - goto
            self.curframe = self.stack[self.curindex][0]
            self.frame_print(self.curindex)
            self.lineno = None
        
    
    def do_up(self, arg):
        # Replace 0 with 2 if up is limited to frames of debugged program
        if self.curindex == 2:
            print 'Initial frame selected; you cannot go up.'
        else:
            self.curindex = self.curindex - 1
            self.curframe = self.stack[self.curindex][0]
            self.frame_print(self.curindex)
            self.lineno = None
    do_u = do_up
    
    def do_down(self, arg):
        if self.curindex + 1 == len(self.stack):
            print 'Bottom (i.e., innermost) frame selected; you cannot go down.'
        else:
            self.curindex = self.curindex + 1
            self.curframe = self.stack[self.curindex][0]
            self.frame_print(self.curindex)
            self.lineno = None
    do_d = do_down
    
    def do_step(self, arg):
        self.set_step()
        return 1
    do_s = do_step
    
    def do_next(self, arg):
        self.set_next(self.curframe)
        return 1
    do_n = do_next
    
    def do_return(self, arg):
        self.set_return(self.curframe)
        return 1
    do_r = do_return
    
    def do_continue(self, arg):
        self.set_continue()
        return 1
    do_c = do_cont = do_continue
    
    def do_quit(self, arg):
        if self.running:
            self.set_quit()
            self.running = 0
        self.alldie = 1
        return 1
    do_q = do_quit

    def do_kill(self, arg):
        self.set_quit()
        self.running = 0
        return 1
    do_k = do_kill
    
    def do_args(self, arg):
        f = self.curframe
        co = f.f_code
        dict = f.f_locals
        n = co.co_argcount
        if co.co_flags & 4: n = n+1
        if co.co_flags & 8: n = n+1
        for i in range(n):
            name = co.co_varnames[i]
            print name, '=',
            if dict.has_key(name): print dict[name]
            else: print "*** undefined ***"
    do_a = do_args
    
    def do_retval(self, arg):
        if self.curframe.f_locals.has_key('__return__'):
            print self.curframe.f_locals['__return__']
        else:
            print '*** Not yet returned!'
    do_rv = do_retval
    
    def do_p(self, arg):
        try:
           l = self.curframe.f_locals
           g = self.curframe.f_globals
        except:
           l = self.outerlocals
           g = self.outerglobals
        g['__privileged__'] = 1
        try:
            value = eval(arg, g, l)
        except:
            return

        print `value`

    def do_list(self, arg):
        if self.filetodebug == "":
            print 'No symbol table is loaded.  Use the "file" command.'
            return
        self.lastcmd = 'list'
        last = None
        if arg:
            try:
                x = eval(arg, {}, {})
                if type(x) == type(()):
                    first, last = x
                    first = int(first)
                    last = int(last)
                    if last < first:
                        # Assume it's a count
                        last = first + last
                else:
                    first = max(1, int(x) - 5)
            except:
                print '*** Error in argument:', `arg`
                return
        elif self.lineno is None:
            if self.running == 0:
                first = 1
            else:
                first = max(1, self.curframe.f_lineno - 5)
        else:
            first = self.lineno + 1
        if last == None:
            last = first + 10
        if self.running == 0:
            filename = self.filetodebug
        else:
            filename = self.curframe.f_code.co_filename
        try:
            for lineno in range(first, last+1):
                line = linecache.getline(filename, lineno)
                if not line:
                    print '[EOF]'
                    break
                else:
                    s = string.rjust(`lineno`, 3)
                    if len(s) < 4: s = s + ' '
                    print s + '\t' + line,
                    self.lineno = lineno
        except KeyboardInterrupt:
            pass
    do_l = do_list

    def do_whatis(self, arg):
        try:
            value = eval(arg, self.curframe.f_globals, \
                    self.curframe.f_locals)
        except:
            print 'No symbol "%s" in current context.' % (arg,)
            return
        code = None
        # Is it a function?
        try: code = value.func_code
        except: pass
        if code:
            print 'type = Function', code.co_name
            return
        # Is it an instance method?
        try: code = value.im_func.func_code
        except: pass
        if code:
            print 'type = Method', code.co_name
            return
        # None of the above...
        print 'type = ' + type(value)
    do_ptype = do_whatis
    
    # Print a traceback starting at the top stack frame.
    # The most recently entered frame is printed last;
    # this is different from dbx and gdb, but consistent with
    # the Python interpreter's stack trace.
    # It is also consistent with the up/down commands (which are
    # compatible with dbx and gdb: up moves towards 'main()'
    # and down moves towards the most recent stack frame).
    
    def print_stack_trace(self):
        try:
            for frame_lineno in self.stack:
                self.print_stack_entry(frame_lineno)
        except KeyboardInterrupt:
            pass
    
    def print_stack_entry(self, frame_lineno, prompt_prefix=line_prefix):
        frame, lineno = frame_lineno
        prefix = ""
        if not self.stop_here(frame) and self.latestbp:
            prefix = "Breakpoint %d, "% (self.latestbp,)
        print self.format_stack_entry(frame_lineno, prefix)

    def format_stack_entry(self, frame_lineno, prefix=""):
        import linecache, repr, string
        frame, lineno = frame_lineno
        filename = frame.f_code.co_filename
        s = ' at ' + filename + ':' + `lineno`
        if frame.f_code.co_name:
            f = frame.f_code.co_name
        else:
            f = "<lambda>"
        if frame.f_locals.has_key('__args__'):
            args = frame.f_locals['__args__']
        else:
            args = None
        if args:
            a = '(' + repr.repr(args) + ')'
        else:
            a = '()'
        first_line = prefix + f + a + s + '\n'
        # Don't want the ?() at <string>:  line printed out; confuses ddd
        if first_line[:15] == '?() at <string>':
            return 'Issue "continue" command'
        second_line = `lineno` + '    ' 
        line = linecache.getline(filename, lineno)
        if line: second_line = second_line + string.strip(line)
        result = first_line + second_line
        if frame.f_locals.has_key('__return__'):
            rv = frame.f_locals['__return__']
            third_line = 'Value returned is $1 = ' + repr.repr(rv)
            result = result + '\n' + third_line
        return result
    
    def do_info(self,args):
        arglist = string.split(args)
        arg = arglist[0]
        if arg == 'program':
            if not self.running:
                print 'The program being debugged is not being run.'
            else:
                print 'Program stopped.\n'
                if self.latestbp:
                    print 'It stopped at breakpoint %d.' % (self.latestbp,)
            return
        if arg == 'line':
            #info line identifier
            if len(arglist) == 2:
                # lineinfo returns (item, file, lineno) or (None,)
                answer = self.lineinfo(arglist[1])
                if answer[0]:
                    print 'Lineinfo <%s> at %s:%s' % answer
                return
            #info line
            if self.lineno == None:
                try:
                    file = self.curframe.f_code.co_filename
                    line = self.curframe.f_lineno
                    print 'Line %s of "%s"' % (line, file)
                except:
                    print 'No line number information available.'
            else:
                print 'Line %s of "%s"' %\
                    (self.lineno, self.curframe.f_code.co_filename)
            return
        if arg == 'files':
            if self.filetodebug == "":
                pass
            else:
                print 'Symbols from "%s".' % (self.filetodebug,)
            return
        if arg == 'breakpoints':
            any = 0
            for b in Breakpoint.bpbynumber:
                if b:
                    if not any:
                        print "Num Type           Disp Enb Where"
                    any = 1
                    b.bpprint()
            if not any:
                print 'No breakpoints or watchpoints.'
            return
        if arg == 'display':
            if not self.display.displayAll():
                print 'There are no auto-display expressions now.'
            return
        print 'Undefined info command: "'+arg+'".  Try "help info".'

    def lineinfo(self, identifier):
        # Input is identifier in single quotes
        idstring = string.split(identifier, "'")
        if len(idstring) == 1:
            # not in single quotes
            id = string.strip(idstring[0])
        elif len(idstring) == 3:
            # quoted
            id = string.strip(idstring[1])
        else:
            return (None,)
        if id == '': return (None,)
        parts = string.split(id, '.')
        if parts[0] == 'self':
            # What could we do here?
            return (None,)
        # Best first guess at file to look at
        try:
            fname = self.curframe.f_code.co_filename
        except:
            fname = self.filetodebug # not tracing yet
        if len(parts) == 1:
            item = parts[0]
        else:
            # more than one part.  First is module, second is method/class
            module = parts[0] + '.py'
            item = parts[1]
            for dirname in sys.path:
                while os.path.islink(dirname):
                    dirname = os.readlink(dirname)
                fname = os.path.join(dirname, module)
                if os.path.exists(fname):
                    break    # take the first
        grepstring = 'egrep -n "(def|class) *%s *[(:]" %s /dev/null' % (item, fname)
        answer = os.popen(grepstring, 'r').readline()
        if answer:
            f, line, junk = string.split(answer, ':', 2)
            return(item, f,line)
        else:
            return (None,)

    def do_run(self, arg):
        # if running, clean up by killing up to outer loop and then run
        if self.running:
           self.cmdqueue = ['k', 'k', 'run '+arg]
           return
        print 'Starting program:'
        if self.filetodebug == "":
           print 'No executable file specified.'
           return
        oldp = self.prompt
        self.prompt = '(pydb) '
        # Mark running, reset command count
        self.running = 1
        # Clear hit counts on breakpoints
        for b in Breakpoint.bpbynumber:
            if b:
                b.hits = 0
        oldarg = sys.argv
        argv = [self.filetodebug]
        if arg:
            args = string.split(arg)
            argv = argv + args
        sys.argv = argv
        self.run('execfile(' + `self.filetodebug` + ')',
               {'__name__': '__main__'})
        sys.argv = oldarg
        self.prompt = oldp

    def do_file(self, fname):
        if fname == "":
            if self.filetodebug == "":
                print 'No exec file now.\nNo symbol file now.'
                return
            else:
                # should confirm this per
                #Discard symbol table from `/work/.../ddd/cxxtest'? (y or n) n
                # XXX 
                # del module
                self.filetodebug == ""
        else:
            path, module = os.path.split(fname)
            module = string.split(module, '.')[0]
            # XXX how to clean up name space ??
            if self.moduletodebug:
                try:
                    # del self.moduletodebug
                    self.moduletodebug = ''
                except:
                    pass
            line = "import " + module + '\n'
            code = compile(line, '<stdin>', 'single')
            exec code in self.outerglobals, self.outerlocals
            # Could print 'Reading symbols from fname...done.' for gdb conformance
            self.filetodebug = fname
            self.moduletodebug = module

    # Ultimately for interrogating settable attributes of the debugger
    def do_show(self, arg):
        if arg == 'language':
            print 'The current source language is "auto; currently python".'

    def do_pwd(self, arg):
        print 'Working directory ' + os.getcwd() + '.'

    def do_cd(self, arg):
        if arg == '':
           do_pwd(arg)
        else:
           os.chdir(arg)
        return

    def do_display(self, arg):
        if not arg:
            # Display anything active
            self.display.displayAny(self.curframe)
        else:
            # Set up a display
            arglist = string.split(arg)
            if len(arglist) == 2:
               (format, variable) = tuple(arglist)
            else:
               format = ""
               variable = arglist[0]
            dp = DisplayNode(self.curframe, variable, format)
            res = dp.checkValid(self.curframe)
            print res
        return
    
    def do_undisplay(self, arg):
        args = string.split(arg)
        for i in args:
            self.display.deleteOne(int(i))

    # Help methods (originally derived from pdb.doc)

    def help_help(self):
        self.help_h()

    def help_h(self):
        print """h(elp)
    Without argument, print the list of available commands.
    With a command name as argument, print help about that command.
    "help exec" gives help on the ! command"""

    def help_where(self):
        self.help_w()

    def help_w(self):
        print """w(here)
    Print a stack trace, with the most recent frame at the bottom."""

    def help_down(self):
        self.help_d()

    def help_d(self):
        print """d(own)
    Move the current frame one level down in the stack trace
    (to an older frame)."""

    def help_up(self):
        self.help_u()

    def help_u(self):
        print """u(p)
    Move the current frame one level up in the stack trace
    (to a newer frame)."""

    def help_break(self):
        self.help_b()

    def help_b(self):
        print """b(reak) [file:lineno] [lineno | method]
    With a line number argument, set a break there in the current
    or specified file.  With a method name, set a break at the first
    executable line after the 'def'.  Without argument, list all breaks."""

    def help_tbreak(self):
        print """tbreak  same arguments as break, but breakpoint is
    removed when first hit."""

    def help_delete(self):
        print """delete [bpnumbers]
    With a space separated list of breakpoint numbers, deletes
    those breakpoints.  Without the argument, deletes all breakpoints."""

    def help_enable(self):
        print """enable [display] bpnumber
    Enables the breakpoints given as a space separated list of bp numbers.
    If the word 'display' is present, the indicated display(s) is/are
    enabled."""

    def help_disable(self):
        print """disable [display] bpnumber
    Disables the breakpoints given as a space separated list of bp numbers.
    If the word 'display' is present, the indicated display(s) is/are
    disabled."""

    def help_ignore(self):
        print """ignore bpnumber count
    Sets the ignore count for the given breakpoint number.  A breakpoint
    becomes active when the ignore count is zero.  When non-zero, the
    count is decremented each time the breakpoint is reached and the
    breakpoint is not disabled and any associated condition evaluates
    to true."""

    def help_condition(self):
        print """condition bpnumber str_condition
    str_condition is a string specifying an expression which
    must evaluate to true before the breakpoint is honored.
    If str_condition is absent, any existing condition is removed;
    i.e., the breakpoint is made unconditional."""

    def help_step(self):
        self.help_s()

    def help_s(self):
        print """s(tep)
    Execute the current line, stop at the first possible occasion
    (either in a function that is called or in the current function)."""

    def help_next(self):
        self.help_n()

    def help_n(self):
        print """n(ext)
    Continue execution until the next line in the current function
    is reached or it returns."""

    def help_return(self):
        self.help_r()

    def help_r(self):
        print """r(eturn)
    Continue execution until the current function returns."""

    def help_continue(self):
        self.help_c()

    def help_cont(self):
        self.help_c()

    def help_c(self):
        print """c(ont(inue))
    Continue execution, only stop when a breakpoint is encountered."""

    def help_list(self):
        self.help_l()

    def help_l(self):
        print """l(ist) [first [,last]]
    List source code for the current file.
    Without arguments, list 11 lines around the current line
    or continue the previous listing.
    With one argument, list 11 lines starting at that line.
    With two arguments, list the given range;
    if the second argument is less than the first, it is a count."""

    def help_args(self):
        self.help_a()

    def help_a(self):
        print """a(rgs)
    Print the arguments of the current function."""

    def help_p(self):
        print """p expression
    Print the value of the expression."""

    def help_exec(self):
        print """(!) statement
    Execute the (one-line) statement in the context of
    the current stack frame.
    The exclamation point can be omitted unless the first word
    of the statement resembles a debugger command.
    To assign to a global variable you must always prefix the
    command with a 'global' command, e.g.:
    (Pdb) global list_options; list_options = ['-l']
    (Pdb)"""

    def help_quit(self):
        self.help_q()

    def help_q(self):
        print """Quit from the debugger."""

    def help_kill(self):
        self.help_k()

    def help_k(self):
        print """Kill the program being debugged
    The program being executed is aborted."""

    def help_info(self):
        print """Generic command for showing things about the program being debugged.

        List of info subcommands:

        info program     -- Execution status of the program
        info line        -- If "info line arg", line number and full path to file containing
                            'def' for arg; if arg omitted, display where currently stopped.
        info display     -- Expressions to display when program stops
        info breakpoints -- Status of user-settable breakpoints
        info files       -- Names of targets and files being debugged"""

    def help_show(self):
        print """Generic command for showing things about the debugger.

        List of show subcommands:

        show language -- Show the current source language """

    def help_cd(self):
        print 'Change to directory specified by argument.'

    def help_display(self):
        print """display [format] expression
        Displays the value of the expression whenever the program stops at
        a breakpoint and the expression is valid in the current namespace
        which must be the one in which the display command was issued.
        Format is /a where a is one of[xotfcs] for hex, octal, binary (twos),
        float, chr and string."""

    def help_undisplay(self):
        print """undisplay dnumber
        Delete the displays listed in the space separated list dnumber."""

    def help_file(self):
        print """Use 'file' as the program to be debugged.  It is the
        program executed when you use the 'run' command."""
    
    def help_frame(self):
        print """frame [arg]
        Prints the current frame location if no argument is given;
        Changes to the frame 'arg' if 'arg' is present.  Frame 0 is
        the newest one."""

    def help_pwd(self):
        print 'Print working directory.'

    def help_run(self):
        print """run [arg1 [arg2 [...]]]
        Run the program to be debugged.  Arguments
        are space separated and are inserted into sys.argv
        following the file name from the 'file' command, all
        for the delectation of the program under study."""
    
    def help_whatis(self):
        print """whatis arg
        Prints the type of the argument."""

    def help_ptype(self):
        print """ptype arg
        Prints the type of the argument."""

    help_pydb = help_help

# Simplified interface

def run(statement, globals=None, locals=None):
    Pdb().run(statement, globals, locals)

def runeval(expression, globals=None, locals=None):
    return Pdb().runeval(expression, globals, locals)

def runctx(statement, globals, locals):
    # B/W compatibility
    run(statement, globals, locals)

def runcall(*args):
    return apply(Pdb().runcall, args)

def set_trace():
    Pdb().set_trace()

# Post-Mortem interface

def post_mortem(t):
    p = Pdb()
    p.reset()
    while t.tb_next <> None: t = t.tb_next
    p.interaction(t.tb_frame, t)

def pm():
    import sys
    post_mortem(sys.last_traceback)


# When invoked as main program, invoke the debugger on a script
if __name__=='__main__':
    import sys
    import os

    outer = Pdb()
    outer.prompt = "(Pdb) "
    outer.alldie = 0
    outer.display = Display()
    outer.reset()
    while not outer.alldie:
        try:
          1 + ''
        except:
          frame = sys.exc_info()[2].tb_frame
          outer.outerlocals = frame.f_locals
          outer.outerglobals = frame.f_locals
        try:
            outer.cmdloop()
        except AttributeError, msg:
            print "Invalid command in outer loop: %s\n" % (msg,)
        except KeyboardInterrupt:
            pass
