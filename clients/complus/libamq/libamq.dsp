# Microsoft Developer Studio Project File - Name="libamq" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libamq - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libamq.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libamq.mak" CFG="libamq - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libamq - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libamq - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libamq - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(INCDIR)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\libamq.lib"

!ELSEIF  "$(CFG)" == "libamq - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(INCDIR)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\libamqd.lib"

!ENDIF 

# Begin Target

# Name "libamq - Win32 Release"
# Name "libamq - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\core\amq_bucket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\core\amq_frames.c
# End Source File
# Begin Source File

SOURCE=..\..\..\core\amq_message.c
# End Source File
# Begin Source File

SOURCE=..\..\..\core\amq_sclient.c
# End Source File
# Begin Source File

SOURCE=..\..\..\core\amq_sclient_agent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\pcre\get.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\icl\icl_mem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\icl\icl_system.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\ipr\ipr_config.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\ipr\ipr_config_table.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\ipr\ipr_dict.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\ipr\ipr_dict_table.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\ipr\ipr_longstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\ipr\ipr_regexp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\ipr\ipr_shortstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\pcre\maketables.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\pcre\pcre.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\pcre\pcreposix.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflbits.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcomp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcons.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflconv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvbs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvdp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvds.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvns.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvsb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvsd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvsn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvtp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflcvts.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfldate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfldescr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfldir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflenv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflfind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflheap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflhttp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflini.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfllang.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfllbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfllist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflmail.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflmath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflmem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflmesg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflmime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflnode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflprint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflproc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflsha.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflsock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflsort.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflsymb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflsyst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfltok.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfltron.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sfluid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflunic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflxml.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflxmll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\sfl\sflxmls.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_cache.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_cache_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_position.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_position_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_reply.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_reply_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_socket_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_socket_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_socket_request_by_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_socket_request_by_thread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_socket_request_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_thread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_thread_handle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_thread_handle_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_thread_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_thread_test.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_timer_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_timer_request_by_thread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base2\smt\smt_timer_request_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\pcre\study.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
