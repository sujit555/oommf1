# FILE: ow.tcl
#
#	OOMMF Widgets
#
# Last modified on: $Date: 2015/03/25 16:45:15 $
# Last modified by: $Author: dgp $
#
# When this version of the ow extension is selected by the 'package require'
# command, this file is sourced.

# Other package requirements
if {[catch {package require Tcl 8}]} {
    package require Tcl 7.5
}
package require Oc 2			;# [Oc_ResolveLink]
if {[catch {package require Tk 8}]} {
    package require Tk 4.1
}
package require Nb 2			;# [Nb_RatApprox]

Oc_CheckTclIndex Ow

# CVS 
package provide Ow 2.0a0

# Provide Oc_Log dialogs for message reporting 
Oc_Log SetLogHandler Ow_Message panic Oc_Log
Oc_Log SetLogHandler Ow_Message error Oc_Log
Oc_Log SetLogHandler Ow_Message warning Oc_Log
Oc_Log SetLogHandler Ow_Message info Oc_Log
Oc_Log SetLogHandler Ow_Message panic
Oc_Log SetLogHandler Ow_Message error
Oc_Log SetLogHandler Ow_Message warning
Oc_Log SetLogHandler Ow_Message info

# Running under X11?
proc Ow_IsX11 {} {
   if {[catch {tk windowingsystem} ws]} {
      # tk windowingsystem command introduced in Tk 8.4
      global tcl_platform
      if {[string match windows $tcl_platform(platform)]} {
         return 0
      } else {
         # Otherwise, presumably X11
         return 1
      }
   }
   if {[string match x11 $ws]} {
      return 1
   }
   return 0
}

# Running under Mac OS X Aqua?
proc Ow_IsAqua {} {
   # tk windowingsystem command introduced in Tk 8.4
   if {![catch {tk windowingsystem} ws] && [string match aqua $ws]} {
      return 1
   }
   return 0
}

# Running under Windows
proc Ow_IsWin32 {} {
   # tk windowingsystem command introduced in Tk 8.4
   if {![catch {tk windowingsystem} ws] && [string match win32 $ws]} {
      return 1
   }
   if {[string compare windows [$tcl_platform(platform)]]==0} {
      return 1
   }
   return 0
}


# Cross-platform mouse bindings.  See NOTES VII, 16-Jan-2014, p. 7 for
# a summary of mouse button events as read by Tk across various
# platforms.  See also the test program oommf/dvl/tkexplore.tcl.
event add <<Ow_LeftButton>> <ButtonPress-1>
if {[Ow_IsAqua]} {
   # Buttons 2 and 3 are reversed on Mac+Aqua
   event add <<Ow_MiddleButton>> <ButtonPress-3>
   event add <<Ow_RightButton>> <ButtonPress-2>
} else {
   event add <<Ow_MiddleButton>> <ButtonPress-2>
   event add <<Ow_RightButton>> <ButtonPress-3>
}
# Extra bindings for systems missing mouse buttons 2 and/or 3
event add <<Ow_RightButton>> <Control-ButtonPress-1>
event add <<Ow_MiddleButton>> <Control-ButtonPress-2>
# Some laptops missing a NumLock key send NumLock enabled by default.
# NumLock shows up on X11 as Mod2, so don't bind against Mod2 on X11.
# On a Mac keyboard, the Option key generates Mod2, so the following
# bindings allow simulation of a middle button press on a Mac keyboard,
# including the cases where the Mac keyboard is controlling a Windows
# session in a virtual machine or another Mac across VNC.
if {![Ow_IsX11]} {
   event add <<Ow_MiddleButton>> <Mod2-ButtonPress-1>
}
# Windows and unix keyboards send Alt from Alt key
event add <<Ow_MiddleButton>> <Alt-ButtonPress-1>
# Alt comes across VNC to a Mac as Mod1.  Mod1 on a Mac
# keyboard is generated by the Command key.  This plus
# the Mod2 binding above means that both Option+B1 and Command+B1
# generate <<Ow_MiddleButton>>
if {[Ow_IsAqua]} {
   event add <<Ow_MiddleButton>> <Mod1-ButtonPress-1>
}
# MISSING CASE: How to generate <<Ow_MiddleButton>> from
# a Mac keyboard across VNC to an X11 session?

# Add parallel bindings for Shift, Release, and Motion events.
foreach btn {Left Middle Right} {
   set seqs [event info "<<Ow_${btn}Button>>"]
   foreach elt $seqs {
      regsub {Button(Press|)} $elt Shift-Button shift_elt
      regsub {Button(Press|)} $elt ButtonRelease release_elt
      regsub {Button(Press|)} $elt Shift-ButtonRelease shift_release_elt
      regsub {Button(Press|)-([0-9])} $elt {B\2-Motion} motion_elt
      regsub {Button(Press|)-([0-9])} $elt {Shift-B\2-Motion} shift_motion_elt
      event add "<<Ow_Shift${btn}Button>>"        $shift_elt
      event add "<<Ow_${btn}ButtonRelease>>"      $release_elt
      event add "<<Ow_Shift${btn}ButtonRelease>>" $shift_release_elt
      event add "<<Ow_${btn}ButtonMotion>>"       $motion_elt
      event add "<<Ow_Shift${btn}ButtonMotion>>"  $shift_motion_elt
   }
}


# Menu bind hack to work around mousewheel problems on X11
if {[Ow_IsX11] \
       && [string match {} [bind Menu <ButtonPress-1>]] \
       && [string match {} [bind Menu <ButtonRelease-1>]]} {
   set bp [bind Menu <ButtonPress>]
   set br [bind Menu <ButtonRelease>]
   if {![string match {} $bp] && ![string match {} $br]} {
      # If code flows to here, then we are on X11, with no bindings on
      # Menu to button 1, but with bindings on Menu to general button.
      # All the restrictions are an attempt to future-proof this hack
      # against changes in the default Tk bindings.
      bind Menu <ButtonPress-1> $bp
      bind Menu <ButtonRelease-1> $br
      bind Menu <ButtonPress> {}
      bind Menu <ButtonRelease> {}
   }
}


# Set up for autoloading of em extension commands
set ow(library) [file dirname [info script]]
if { [lsearch -exact $auto_path $ow(library)] == -1 } {
    lappend auto_path $ow(library)
}

# Load in any local modifications
set local [file join [file dirname [info script]] local ow.tcl]
if {[file isfile $local] && [file readable $local]} {
    uplevel #0 [list source $local]
}


### MouseWheel support   ###############################################
### Based on coded posted to http://paste.tclers.tk/tcl by "de" on
### Wed Mar 19 00:37:18 GMT 2008.  The <Shift-MouseWheel> binding to
### horizontal scroll reportedly mimics standard mousewheel bindings
### on Mac OS X.
global Ow_mw_classes Ow_mw_scaling
set Ow_mw_classes [list Text Canvas Listbox Table TreeCtrl]
set Ow_mw_scaling 9999

proc Ow_DefaultScrollHandler { w D horizontal } {
   # scrollbars have different call conventions
   if {[string match "Scrollbar" [winfo class $w]]} {
      # Ignore "horizontal" import; scrollbars presumably know
      # which way to scroll.
      switch -glob -- [$w cget -orient] {
	 h* { eval [$w cget -command] scroll $D units }
	 v* { eval [$w cget -command] scroll $D units }
      }
   } else {
      if {$horizontal} {
	 set dir "x"
      } else {
	 set dir "y"
      }
      while {![string match {} $w]} {
	 global Ow_mw_classes
	 if {[lsearch $Ow_mw_classes [winfo class $w]] > -1} {
	    if {[string match {} [$w cget -${dir}scrollcommand]]} {
	       set w [winfo parent $w]
	       continue
	    }
	    catch {$w ${dir}view scroll $D units}
	    break
	 }
	 set w [winfo parent $w]
      }
   }
}

proc Ow_MouseWheel { handler wFired D X Y horizontal} {
   # do not double-fire in case the class already has a binding
   if {![string match {} [bind [winfo class $wFired] <MouseWheel>]]} { return }

   # Scaling.  On Windows the smallest increment the mouse wheel
   # generates is +/-120; the base increment for touchpads appears to
   # be +/-8.  On Unix and Mac the smallest increment is +/-1.  Use
   # dynamic adjustment to handle all cases.
   if {$D == 0} { return }
   global Ow_mw_scaling
   if {abs($D)<$Ow_mw_scaling} {
      set Ow_mw_scaling [expr {abs($D)}]
   }
   set D [expr {round(double(-4*$D)/$Ow_mw_scaling)}] ;# round returns an int
   # obtain the window the mouse is over
   set w [winfo containing $X $Y]
   # if we are outside the app, try and scroll the focus widget
   if {![winfo exists $w]} { catch {set w [focus]} }
   if {[winfo exists $w]} {
      $handler $w $D $horizontal 
   }
}


if {[Oc_Application CompareVersions [info tclversion] 8.1]>=0} {
   # MouseWheel event introduced in Tk 8.1

   # Clear default bindings
   foreach class $Ow_mw_classes { bind $class <MouseWheel> {} }
   if {[Ow_IsX11]} {
      foreach class $Ow_mw_classes {
         bind $class <4> {}
         bind $class <5> {}
      }
   }

   # Handler setup proc
   proc Ow_BindMouseWheelHandler { win {procname {}} } {
      if {[string match {} $procname]} { set procname Ow_DefaultScrollHandler }
      if {[Ow_IsX11]} {
	 # Support for mousewheels on Linux/Unix commonly comes through
	 # mapping the wheel to the extended buttons.
	 bind $win <4> [list Ow_MouseWheel $procname %W 1 %X %Y 0]
	 bind $win <5> [list Ow_MouseWheel $procname %W -1 %X %Y 0]
	 bind $win <Shift-4> [list Ow_MouseWheel $procname %W 1 %X %Y 1]
	 bind $win <Shift-5> [list Ow_MouseWheel $procname %W -1 %X %Y 1]
      }
      bind $win <MouseWheel> [list Ow_MouseWheel $procname %W %D %X %Y 0]
      bind $win <Shift-MouseWheel> [list Ow_MouseWheel $procname %W %D %X %Y 1]
   }

   # Bind "all" to mouse wheel
   Ow_BindMouseWheelHandler all
} else {
   proc Ow_BindMouseWheelHandler { win {procname {}} } {}   ;# NOP
}
