// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once
#define STRICT

//!	�����ɂ���x��
#pragma warning( disable : 4100 )	//!<	�����͔����̖{�̕��� 1 �x���Q�Ƃ���܂���B
//#pragma warning( disable : 4101 )	//!<	���[�J���ϐ��� 1 �x���g���Ă��܂���B
#pragma warning( disable : 4201 )	//!<	��W���̊g���@�\���g�p����Ă��܂�
#pragma warning( disable : 4244 )	//!<	�^�ϊ��ɂ�����A�f�[�^��������\���ɂ��āB
#pragma warning( disable : 4312 )	//!<	���傫���T�C�Y�ւ̌^�ϊ��ɂ���
//#pragma warning( disable : 4995 )	//!<	���O��������ꂽ #pragma �Ƃ��ċL�q����Ă��܂��B

#include "targetver.h"

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WIN32_LEAN_AND_MEAN		//	Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
// Windows �w�b�_�[ �t�@�C��:
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <windowsX.h>

//	�V�F���Ƃ�
#pragma comment(lib, "shell32.lib")

//	�R�����_�C�����O
#include <commdlg.h>

#include <Commctrl.h>
#pragma comment(lib, "ComCtl32.lib")

#ifndef _ORCOLL

//	IME�̑���Ɏg��
#include <imm.h>
#pragma comment(lib, "imm32.lib")

//	SQLite3
#include "sqlite3.h"
#pragma comment(lib, "sqlite3.lib")

//	�f�o�b�O�p���b�Z�[�W
#ifdef _DEBUG
#define SQL_DEBUG(db)	SqlErrMsgView(db,__LINE__)
#else
#define SQL_DEBUG(db)
#endif

#endif

// C �����^�C�� �w�b�_�[ �t�@�C��
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include <memory.h>
#include <tchar.h>

#define STRSAFE_NO_CB_FUNCTIONS
#include <strsafe.h>

#pragma warning( disable : 4995 )
#include <shlwapi.h>
#pragma warning( default : 4995 )
#pragma comment(lib, "shlwapi.lib")

//-------------------------------------------------------------------------------------------------

#pragma warning( disable : 4995 )	//	���O��������ꂽ #pragma �Ƃ��ċL�q����Ă��܂��B
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#pragma warning( default : 4995 )

using namespace	std;	//	���̃X�R�[�v���ł�std::���ȗ��ł���
//-------------------------------------------------------------------------------------------------

//���̃A�v����GUID�A���ɈӖ��͂Ȃ�
//	{66D3E881-972B-458B-935E-9E78B926B415}
static CONST GUID gcstGUID = { 0x66D3E881, 0x972B, 0x458B, { 0x93, 0x5E, 0x9E, 0x78, 0xB9, 0x26, 0xB4, 0x15 } };
//-------------------------------------------------------------------------------------------------

//#define OPEN_HISTORY

//#define FIND_STRINGS	//	�����񌟍��@�\
#define FIND_MAA_FILE	//	MAA�t�@�C�������@�\

//�@�\�n�j
#define  MOZI_SCRIPT	//	�T�O��Script�@�\
#define CONTEXT_EDIT	//	�E�N�����j���[�ҏW
#define MAA_PROFILE		//	�c���[�\�����v���t�@�C������@�\
#define MULTI_FILE		//	�����t�@�C���̓����W�J

#define USE_NOTIFYICON	//	�^�X�N�g���C�A�C�R����L��
//-------------------------------------------------------------------------------------------------

//	�P�Ȃ鎯�ʖ��@extern�ŊO���Q�Ƃ���Ă�ϐ��ɂ������Ă���
#define EXTERNED
//-------------------------------------------------------------------------------------------------

#ifdef _DEBUG

#define TRACE(str,...)	OutputDebugStringPlus( GetLastError(), __FILE__, __LINE__, __FUNCTION__, str, __VA_ARGS__ )

VOID	OutputDebugStringPlus( DWORD, LPSTR, INT, LPSTR, LPTSTR, ... );	//!<	

VOID	SqlErrMsgView( sqlite3 *, DWORD );

#else
	#define TRACE(x,...)
#endif

#define FREE(pp)	{if(pp){free(pp);pp=NULL;}}

//�X�e�[�^�X�o�[������ǉ��}�N��
#define StatusBar_SetText(hwndSB,ipart,ptext)	(BOOLEAN)SNDMSG((hwndSB),SB_SETTEXT,ipart,(LPARAM)(LPCTSTR)(ptext))
//-------------------------------------------------------------------------------------------------

#define MIN_STRING	20
#define SUB_STRING	64
#define MAX_STRING	130
#define BIG_STRING	520

//	�E�C���h�E�T�C�Y
#define W_WIDTH		480
#define W_HEIGHT	400

//	�E�C���h�E�T�C�Y
#define WCL_WIDTH	480
#define WCL_HEIGHT	320

//	�h�b�L���O�T�C�Y
#define PLIST_DOCK	190
#define TMPL_DOCK	150
//-------------------------------------------------------------------------------------------------

#define WMP_TRAYNOTIFYICON	(WM_APP+1)
#define WMP_BRUSH_TOGGLE	(WM_APP+2)
#define WMP_PREVIEW_CLOSE	(WM_APP+3)
//-------------------------------------------------------------------------------------------------

#define BASIC_COLOUR	RGB(0xF0,0xF0,0xF0)	//	Collector�p
//-------------------------------------------------------------------------------------------------

#define USER_ITEM_FILE	TEXT("UserItem.ast")
#define USER_ITEM_MAX	16

#define AA_BRUSH_FILE	TEXT("aabrush.txt")		//	�h��Ԃ��p
#define AA_LIST_FILE	TEXT("aalist.txt")		//	��s�e���v���[�g

#define MAA_FAVDB_FILE	TEXT("Favorite.qmt")	//	AA���X�g�p
#define MAA_TREE_CACHE	TEXT("TreeCache.qor")	//	�c���[�̒��g������Ă����E�g��Ȃ�

#define NAMELESS_DUMMY	TEXT("NoName0.txt")
#define NAME_DUMMY_NAME	TEXT("NoName")
#define NAME_DUMMY_EXT	TEXT("txt")

#define TEMPLATE_DIR	TEXT("Templates")
#define BACKUP_DIR		TEXT("BackUp")
#define PROFILE_DIR		TEXT("Profile")
//-------------------------------------------------------------------------------------------------

//	�g�̐�	20110707	�g���P�O�ɑ��₵��
#define FRAME_MAX	10

//	�ݒ�ɂ���
#define INI_FILE		TEXT("Utuho.ini")
#define FRAME_INI_FILE	TEXT("Satori.ini")
#define MZCX_INI_FILE	TEXT("Koisi.ini")



#define INIT_LOAD		1
#define INIT_SAVE		0

#define WDP_MVIEW		1
#define WDP_PLIST		2
#define WDP_LNTMPL		3
#define WDP_BRTMPL		4
#define WDP_MAATPL		5
#define WDP_PREVIEW		6

#define FONTSZ_NORMAL	16
#define FONTSZ_REDUCE	12

#define VL_CLASHCOVER	0	//	�N��������P�A����Ƃ��ɂO�ɂ��āACLASH�����m
#define VL_GROUP_UNDO	1	//	�O���[�v�A���h�D
#define VL_USE_UNICODE	2	//	���j�R�[�h�p�f�B���O
#define VL_LAYER_TRANS	3	//	���C���{�b�N�X�����x
#define VL_RIGHT_SLIDE	4	//	�E�񂹂�ʒu
#define VL_MAA_SPLIT	5	//	�l�`�`�̃X�v���b�g�o�[�̈ʒu
#define VL_SETMETHOD	6	//	�l�`�`�ꗗ�ŃN���b�R�����Ƃ��̕W������
#define VL_UNILISTLAST	7	//	���j�R�[�h�ꗗ������Ƃ��̈ʒu
#define VL_MAATIP_VIEW	8	//	�l�`�`�̂`�`�c�[���`�b�v�̕\�����邩
#define VL_MAATIP_SIZE	9	//	�l�`�`�̂`�`�c�[���`�b�v�̕����T�C�Y�E16��12
#define VL_LINETMP_CLM	10	//	��s�e���v���̃J������
#define VL_BRUSHTMP_CLM	11	//	�u���V�e���v���̃J������
#define VL_UNIRADIX_HEX	12	//	���j�R�[�h���l�Q�Ƃ��P�U�i���ɂ��邩�ǂ���
#define VL_BACKUP_INTVL	13	//	�o�b�N�A�b�v���o�E�f�t�H�T�����炢�H
#define VL_BACKUP_MSGON	14	//	�o�b�N�A�b�v�����Ƃ��̃��b�Z�[�W��\�����邩�H
#define VL_GRID_X_POS	15	//	�O���b�h���̂w�h�b�g��
#define VL_GRID_Y_POS	16	//	�O���b�h���̂x�h�b�g��
#define VL_MAA_TOPMOST	17	//	�l�`�`���őO�ʁE�r���[����p
#define VL_R_RULER_POS	18	//	�E���[���̈ʒu
#define VL_CRLF_CODE	19	//	���s�R�[�h�F�O������΁E��O�x�x
//#define VL_SPACE_VIEW	20	//	�󔒂�\�����邩�@��O�\��
#define VL_GRID_VIEW	21	//	�O���b�h�\�����邩
#define VL_R_RULER_VIEW	22	//	�E���[���\�����邩
#define VL_PAGETIP_VIEW	23	//	�ňꗗ�̃c�[���`�b�v�\�����邩
#define VL_PCOMBINE_NM	24	//	�P�Ȃ瓝��Message����
#define VL_PDIVIDE_NM	25	//	�P�Ȃ番��Message����
#define VL_PDELETE_NM	26	//	�P�Ȃ�폜Message����
#define VL_MAASEP_STYLE	27	//	�����e���v���E�P�Ȃ��؂���X�^�C��
#define VL_USE_BALLOON	28	//	�ۑ��m�F���b�Z�[�W�\�����邩�ǂ���
#define VL_CLIPFILECNT	29
#define VL_PLS_LN_DOCK	30	//	�ňꗗ���͂�������
//#define VL_BRUSH_DOCK	31	//	��s�EBrush�e���v�����͂�������
#define VS_PROFILE_NAME	32	//	
#define VS_PAGE_FORMAT	33	//	�Ŕԍ��}���̕�����e���v��
#define VL_SWAP_COPY	34	//	�R�s�[�W����SJIS�ɂ��邩

#define CLRV_BASICPEN	101
#define CLRV_BASICBK	102
#define CLRV_GRIDLINE	103
#define CLRV_CRLFMARK	104
#define CLRV_CANTSJIS	105
//-------------------------------------------------------------------------------------------------

//	�X�e�[�^�X�o�[�̓�
#define SB_MODIFY	0	//	�ύX
#define SB_OP_STYLE	1	//	�I�y��
#define SB_MOUSEPOS	2	//	�}�E�X�J�[�\���ʒu
#define SB_CURSOR	3	//	�J�[�\���ʒu�̃h�b�g�l�Ƃ�
#define SB_LAYER	4	//	���C���{�b�N�X�̈ʒu
#define SB_BYTECNT	5	//	�o�C�g��
#define SB_SELBYTE	6	//	�I��͈͂̃o�C�g��
//-------------------------------------------------------------------------------------------------

//	���ԍ�
#define WND_MAIN	1
#define WND_MAAT	2
#define WND_PAGE	3
#define WND_LINE	4
#define WND_BRUSH	5
#define WND_TAIL	5	//	���[�_�~�[

//	���[�h
#define M_DESTROY	0
#define M_CREATE	1
#define M_EXISTENCE	2

#define MAA_DEFAULT		0xFF
#define MAA_INSERT		0
#define MAA_INTERRUPT	1
#define MAA_LAYERED		2
#define MAA_UNICLIP		3
#define MAA_SJISCLIP	4


//	�w���R�[�h�E���Ԃ�Ȃ��悤��
#define D_SJIS		0x00	//	�V�t�gJIS
#define D_UNI		0x01	//	���j�R�[�h
#define D_SQUARE	0x02	//	��`�I��
#define D_INVISI	0x10	//	�s����Ԃ̓��ʏ���
#define D_RENAME	0x80	//	���O��t���ĕۑ�
//-------------------------------------------------------------------------------------------------

//	ImgCtl.dll�ɏ���
#define ISAVE_BMP	0x1
//#define ISAVE_JPEG	0x2
#define ISAVE_PNG	0x3
//-------------------------------------------------------------------------------------------------

#define CLIP_FORMAT	TEXT("ORINRIN_EDITOR_STYLE")	//!<	���ʗp�E���ɈӖ��͂Ȃ�
#define CLIP_SQUARE	TEXT("MSDEVColumnSelect")	//!<	��`�I�����ʎq�EVC�̂��
//-------------------------------------------------------------------------------------------------


//!	�L�[�R�[�h���蓖��
#define	VK_0	0x30
#define	VK_1	0x31
#define	VK_2	0x32
#define	VK_3	0x33
#define	VK_4	0x34
#define	VK_5	0x35
#define	VK_6	0x36
#define	VK_7	0x37
#define	VK_8	0x38
#define	VK_9	0x39

//	�K�v�ȃ{�^���ɖ��O�t�������Ă���
#define	VK_A	0x41
#define	VK_B	0x42
#define	VK_C	0x43
#define	VK_D	0x44
#define	VK_E	0x45
#define	VK_F	0x46
#define	VK_G	0x47
#define	VK_H	0x48
#define	VK_I	0x49
#define	VK_J	0x4A
#define	VK_K	0x4B
#define	VK_L	0x4C
#define	VK_M	0x4D
#define	VK_N	0x4E
#define	VK_O	0x4F
#define	VK_P	0x50
#define	VK_Q	0x51
#define	VK_R	0x52
#define	VK_S	0x53
#define	VK_T	0x54
#define	VK_U	0x55
#define	VK_V	0x56
#define	VK_W	0x57
#define	VK_X	0x58
#define	VK_Y	0x59
#define	VK_Z	0x5A
//-------------------------------------------------------------------------------------------------