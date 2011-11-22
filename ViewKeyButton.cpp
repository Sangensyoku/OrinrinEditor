/*! @file
	@brief �L�[�{�[�h���͂�}�E�X�N���b�N�̖ʓ|�݂�
	���̃t�@�C���� ViewKeyButton.cpp �ł��B
	@author	SikigamiHNQ
	@date	2011/04/22
*/

#include "stdafx.h"
#include "OrinrinEditor.h"
//-------------------------------------------------------------------------------------------------

/*
MK_CONTROL	Ctrl �L�[��������Ă���ꍇ�ɐݒ肵�܂��B
MK_LBUTTON	�}�E�X�̍��{�^����������Ă���ꍇ�ɐݒ肵�܂��B
MK_MBUTTON	�}�E�X�̒����{�^����������Ă���ꍇ�ɐݒ肵�܂��B
MK_RBUTTON	�}�E�X�̉E�{�^����������Ă���ꍇ�ɐݒ肵�܂��B
MK_SHIFT	Shift �L�[��������Ă���ꍇ�ɐݒ肵�܂��B
*/
//-------------------------------------------------------------------------------------------------

extern HWND		ghPrntWnd;		//!<	�e�E�C���h�E�n���h��
extern HWND		ghViewWnd;		//!<	���̃E�C���h�E�̃n���h��

extern INT		gdXmemory;		//		���O�̂w�ʒu���o���Ă���

extern INT		gdDocXdot;		//!<	�L�����b�g�̂w�h�b�g�E�h�L�������g�ʒu
extern INT		gdDocLine;		//!<	�L�����b�g�̂x�s���E�h�L�������g�ʒu
extern INT		gdDocMozi;		//!<	�L�����b�g�̍����̕�����

//	��ʃT�C�Y���m�F���āA�ړ��ɂ��X�N���[���̖ʓ|�݂�
extern INT		gdHideXdot;		//!<	���̉B�ꕔ��
extern INT		gdViewTopLine;	//!<	�\�����̍ŏ㕔�s�ԍ�
extern SIZE		gstViewArea;	//!<	�\���̈�̃T�C�Y�E���[���[���̗̈�͖���
extern INT		gdDispingLine;	//!<	�����Ă�s���E���r���[�Ɍ����Ă閖�[�͊܂܂Ȃ�

extern BOOLEAN	gbExtract;	

extern  UINT	gbUniPad;		//!<	�p�f�B���O�Ƀ��j�R�[�h�������āA�h�b�g�������Ȃ��悤�ɂ���

//	�����̃L�[�̋�́AGetKeyState��������GetKeyboardState���g���΂���
EXTERNED BOOLEAN	gbShiftOn;	//!<	�V�t�g��������Ă���
EXTERNED BOOLEAN	gbCtrlOn;	//!<	�R���g���[����������Ă���
EXTERNED BOOLEAN	gbAltOn;	//!<	�A���^��������Ă���

EXTERNED POINT	gstCursor;		//!<	�������l�����Ȃ��ACursor�̃h�b�g���s�ʒu

EXTERNED UINT	gbBrushMode;	//!<	���u���V���[�h
static TCHAR	gatBrushPtn[SUB_STRING];	//!<	�u���V�p���[��

static  UINT	gdSqFillCnt;	//!<	��`�I�����AIME������œh��Ԃ������̕�����
//-------------------------------------------------------------------------------------------------

HRESULT	ViewBrushFilling( VOID );
//-------------------------------------------------------------------------------------------------


/*!
	�V�t�g�A�R���g���[���A�A���g�L�[�̏�Ԃ��m�F����
*/
VOID ViewCombiKeyCheck( VOID )
{
//	SHIFT,CONTROL,ALT�̃L�[�̋�́AGetKeyState��������GetKeyboardState���g���΂���
	gbShiftOn = (0x8000 & GetKeyState(VK_SHIFT)) ? TRUE : FALSE;
	gbCtrlOn  = (0x8000 & GetKeyState(VK_CONTROL)) ? TRUE : FALSE;
	gbAltOn   = (0x8000 & GetKeyState(VK_MENU)) ? TRUE : FALSE;

	return;
}
//-------------------------------------------------------------------------------------------------

/*!
	�ҏW�r���[�̃L�[�_�E��������
	@param[in]	hWnd	�E�C���h�E�n���h���E�r���[�̂Ƃ͌���Ȃ��̂Œ��ӃZ��
	@param[in]	vk		�����ꂽ�L�[�����z�L�[�R�[�h�ŗ���
	@param[in]	fDown	��O�_�E���@�O�A�b�v
	@param[in]	cRepeat	�A���I�T���񐔁E���ĂȂ��H
	@param[in]	flags	�L�[�t���O���낢��
	@return		����
*/
VOID Evw_OnKey( HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags )
{
	INT		bXdirect = 0;	//	�w�̈ړ�����
	UINT	dXwidth;	//	�w�̈ړ��h�b�g
	INT		dDot, bCrLf, iLines, i;
	BOOLEAN	bJump = FALSE, bMemoryX = FALSE;
	BOOLEAN	bSelect = FALSE;
	UINT	bSqSel  = 0;

	ViewCombiKeyCheck(  );

	ViewSelPositionSet( NULL );	//	���쒼�O�̈ʒu
	//	���Ń��[���[�̃h���[���������Ă�BCtrl�Ƃ��������ςł����

	if( fDown )
	{
		switch( vk )
		{
			case VK_RIGHT:	bXdirect =  1;	bMemoryX = TRUE;	break;

			case VK_LEFT:	bXdirect = -1;	bMemoryX = TRUE;	break;

			case VK_DOWN:	//	�s������
				gdDocLine++;
				dDot = DocLineParamGet( gdDocLine, NULL, NULL );	//	���̍s�����������玀�ɂ܂�
				if( -1 == dDot ){	gdDocLine--;	}	//	�߂��Ă���
				break;

			case VK_UP:	//	�s�����
				if( 0 < gdDocLine ){	gdDocLine--;	}	//	�O�Ȃ�ω�����
				break;

			case VK_PRIOR:	//	PageUp
				gdDocLine -= 10;
				if( 0 > gdDocLine ){	gdDocLine = 0;	}
				break;

			case VK_NEXT:	//	PageDown
				gdDocLine += 10;
				iLines = DocPageParamGet( NULL, NULL );
				if( iLines <= gdDocLine ){	gdDocLine =  iLines - 1;	}
				break;

			case VK_END:	//	Ctrl+End�Ŗ�����
				if( gbCtrlOn ){	gdDocLine = DocPageParamGet( NULL, NULL ) - 1;	}
				gdDocXdot = DocLineParamGet( gdDocLine, &gdDocMozi, NULL );
				bMemoryX = TRUE;
				break;

			case VK_HOME:	//	Ctrl+Home�Ő擪��
				gdDocXdot = 0;	gdDocMozi = 0;	bMemoryX = TRUE;
				if( gbCtrlOn  ){	gdDocLine = 0;	}
				break;


			case VK_DELETE:
				bSelect = IsSelecting( &bSqSel );
				iLines = DocPageParamGet( NULL, NULL );
				if( bSelect )	//	�I����ԂȂ�A���������폜����
				{
					bCrLf = DocSelectedDelete( &gdDocXdot , &gdDocLine, bSqSel );
				}
				else
				{
					bCrLf = DocInputDelete( gdDocXdot , gdDocLine );
				}
				if( bCrLf  )	//	���������s�ȍ~�S����ւ�
				{
					for( i = gdDocLine; iLines > i; i++ ){	ViewRedrawSetLine(  i );	}
				}
				else{	ViewRedrawSetLine( gdDocLine  );	}
				ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�L�����b�g�ʒu�����߂�
				DocPageInfoRenew( -1, 1 );
				bMemoryX = TRUE;
				return;

			case VK_PROCESSKEY:
				//	����{�隠�I����ϊ������Ă�Ƃ��̓L�[���͂͑S�������ɂ���E�V�t�g�Ƃ��͕ʂ��ۂ�
				return;

			default:
		//		TRACE( TEXT("vk[0x%04X]"), vk );
				return;
		}
		//	IME��ON�̂Ƃ��̃I�T���́Avk�F0xE5�A���E�m�肳�ꂽ������CHAR��

		if( bMemoryX )	gdXmemory = gdDocXdot;
		else			gdDocXdot = gdXmemory;

		//	�L�����b�g�ʒu����
		DocLetterPosGetAdjust( &gdDocXdot, gdDocLine, 0 );
		//	�����ŕ����ʒu�̃C���N���E�f�N���̖ʓ|�݂āA�h�b�g�ƍs�ʒu��ύX
		dXwidth = DocLetterShiftPos( gdDocXdot, gdDocLine, bXdirect, NULL, &bJump );

		if( 0 > bXdirect )	//	���E�擪�֌�������
		{
			gdDocXdot -= dXwidth;
			if( 0 >  gdDocXdot )	gdDocXdot = 0;

			if( bJump )
			{
				if( 0 < gdDocLine ){	gdDocLine--;	}	//	�O�Ȃ�ω�����

				//	�ׂ̍s�̖����Ɉړ�����
				dDot = DocLineParamGet( gdDocLine, NULL, NULL );
				gdDocXdot = dDot;
			}
		}

		if( 0 < bXdirect )	//	�E�E�����֌�������
		{
			gdDocXdot += dXwidth;
			if( bJump )
			{
				gdDocLine++;

				//	���̍s������������
				dDot = DocLineParamGet( gdDocLine, NULL, NULL );
				if( -1 == dDot ){	gdDocLine--;	}	//	�߂��Ă���
				else{	gdDocXdot = 0;	}	//	���̍s�ֈړ����čs����
			}
		}

		gdDocMozi = DocLetterPosGetAdjust( &gdDocXdot, gdDocLine, 0 );

		ViewSelMoveCheck( FALSE );	//	�ʒu���܂�����A�I����Ԃ�Check

		if( bMemoryX )	gdXmemory = gdDocXdot;

		ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�ŏI�I�Ȉʒu�ɃL�����b�g�ʒu��ύX
	}
	else	//	�L�[������
	{
		//	�Ȃ��H
	}

	return;
}
//-------------------------------------------------------------------------------------------------

/*!
	�ҏW�r���[�̕����L�[�I�T��������
	@param[in]	hWnd	�E�C���h�E�n���h���E�r���[�̂Ƃ͌���Ȃ��̂Œ��ӃZ��
	@param[in]	ch		�����ꂽ����
	@param[in]	cRepeat	�L�[���s�[�g�񐔁E�����ĂȂ��H
	@return		����
*/
VOID Evw_OnChar( HWND hWnd, TCHAR ch, INT cRepeat )
{
	BOOLEAN	bSelect;
	UINT	bSqSel = 0;
	INT		isctrl, bCrLf, iLines, i;
	//	�o�b�N�X�y�[�X�Ƃ����s0x0D������
	TCHAR	atCh[2];


	ViewCombiKeyCheck(  );

	bSelect = IsSelecting( &bSqSel );	//	�I����Ԃł��邩	��`�I�𒆂ł��邩

	isctrl = iswcntrl( ch );	//	���䕶���ł��邩
	//	Ctrl+Z�Ƃ��͐��䕶���ŗ���̂Œ���
	if( isctrl )	//	���䕶����BS��Return�ȊO�͖����ł�����
	{
		TRACE( TEXT("���䕶��[%04X]"), ch );

		//	TAB�͂������ɗ���

		if( VK_RETURN == ch )	//	���s
		{
			DocCrLfAdd( gdDocXdot , gdDocLine, TRUE );
			ViewRedrawSetLine( gdDocLine );

			gdDocXdot = 0;	gdDocMozi = 0;	gdDocLine++;	//	���̍s�Ɉڂ�
			ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�ʒu�����߂�
			gdXmemory = gdDocXdot;	//	�ŐV�ʒu�L��
			//	���s�����s�ȍ~�S����ւ�
			iLines = DocPageParamGet( NULL, NULL );
			for( i = gdDocLine; iLines > i; i++ ){	ViewRedrawSetLine(  i );	}
		}

		if( VK_BACK == ch )	//	BackSpace
		{
			TRACE( TEXT("BACKSP [%d][%d:%d]"), bSelect, gdDocXdot, gdDocLine );
			iLines = DocPageParamGet( NULL, NULL );
			if( bSelect )	//	�I����ԂȂ�A���������폜����
			{
				bCrLf = DocSelectedDelete( &gdDocXdot , &gdDocLine, bSqSel );
			}
			else
			{
				bCrLf = DocInputBkSpace( &gdDocXdot , &gdDocLine );
			}
			if( bCrLf  )	//	���������s�ȍ~�S����ւ�
			{
				for( i = gdDocLine; iLines > i; i++ ){	ViewRedrawSetLine(  i );	}
			}
			else{	ViewRedrawSetLine( gdDocLine  );	}

			ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�L�����b�g�ʒu�����߂�
			gdXmemory = gdDocXdot;	//	�ŐV�ʒu�L��
		}

		return;
	}

	TRACE( TEXT("���͕���[%c]"), ch );

	if( 0 < gdSqFillCnt )	//	IME�o�R�ŋ�`�h��ׂ����������Ă���
	{
		gdSqFillCnt--;
		TRACE( TEXT("�L�����Z��[%u]"), gdSqFillCnt );
		return;
	}

	iLines = DocPageParamGet( NULL, NULL );
	bCrLf = 0;
	if( bSelect )	//	�I����ԂŗL��ꍇ�E��`�Ȃ�h��ׂ��A�ʏ�Ȃ�폜
	{
		if( bSqSel )	//	��`�I�𒆂Ȃ�h��Ԃ��ďI��
		{
			atCh[0] = ch;	atCh[1] = NULL;
			DocSelectedBrushFilling( atCh, &gdDocXdot ,&gdDocLine );
			ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�L�����b�g�ʒu�����߂�
			DocPageInfoRenew( -1, 1 );
			return;
		}
		else{	bCrLf = DocSelectedDelete( &gdDocXdot , &gdDocLine, 0 );	}
		//	�ʏ�I�𒆂Ȃ�A��U���͈̔͂��폜����
	}

	DocInsertLetter( &gdDocXdot, gdDocLine, ch );	//	�s�ɒǉ�
	//	���ŃA���h�D�o�b�t�@�����O
	gdDocMozi = DocLetterPosGetAdjust( &gdDocXdot, gdDocLine, 0 );
	ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�ʒu�ړ�

	gdXmemory = gdDocXdot;	//	�ŐV�ʒu�L��

	if( bCrLf  )	//	���������s�ȍ~�S����ւ�
	{
		for( i = gdDocLine; iLines > i; i++ ){	ViewRedrawSetLine(  i );	}
	}
	else{	ViewRedrawSetLine( gdDocLine  );	}

	DocPageInfoRenew( -1, 1 );

	return;
}
//-------------------------------------------------------------------------------------------------


/*!
	�r���[�Ń}�E�X�̍��{�^�����_�E�����ꂽ�Ƃ�
	@param[in]	hWnd			�E�C���h�E�n���h���E�r���[�̂Ƃ͌���Ȃ��̂Œ��ӃZ��
	@param[in]	fDoubleClick	��O�_�u���N���b�N���ꂽ�ꍇ
	@param[in]	x				���������w���W�l
	@param[in]	y				���������x���W�l
	@param[in]	keyFlags		���ɉ�����Ă�L�[�ɂ���
*/
VOID Evw_OnLButtonDown( HWND hWnd, BOOL fDoubleClick, INT x, INT y, UINT keyFlags )
{
	INT		dX, dY;	//	
	INT		dDot, dMaxDot, dLine, iMaxLine;	//	

	SetFocus( hWnd );	//	�}�E�X�C���Ńt�H�[�J�X

	dX = x;
	dY = y;
	ViewPositionTransform( &dX, &dY, 0 );	//	�����ŁA�h�L�������g�ʒu�ɕύX

	ViewCombiKeyCheck(  );


	//	�}�C�i�X�̂Ƃ��̓��[���[���s�ԍ��G���A
	if( 0 > dX )	dX = 0;
	if( 0 > dY )	dY = 0;
	
	dDot  = dX;
	dLine = dY / LINE_HEIGHT;

	if( fDoubleClick )	//	�_�u���N���b�N
	{
		TRACE( TEXT("�}�E�X���_�u���N���b�N[%d / %d]%d:%d:%d"), dDot, dLine, gbShiftOn, gbCtrlOn, gbAltOn );


		return;	//	����ȍ~�͏������Ȃ��ėǂ��͂�
	}

	TRACE( TEXT("�}�E�X���_�E��[%d / %d]%d:%d:%d"), dDot, dLine, gbShiftOn, gbCtrlOn, gbAltOn );

	SetCapture( hWnd  );	//	�}�E�X�L���v�`��

	//	�L���Ȉʒu���ǂ����m�F

	//	�s���m�F���āA�͂ݏo���Ă��疖�[�ɂ��Ă���
	iMaxLine = DocPageParamGet( NULL, NULL );
	if( iMaxLine <=dLine )	dLine = iMaxLine - 1;

	//	���̍s�̃h�b�g�����m�F���āA�͂ݏo���Ă��疖�[�ɂ��Ă���
	dMaxDot = DocLineParamGet( dLine, NULL, NULL );
	if( dMaxDot <=dDot )	dDot = dMaxDot;

	//	�����ʒu�ɍ��킹�Ē���
	gdDocMozi = DocLetterPosGetAdjust( &dDot, dLine, 0 );
	gdDocXdot = dDot;
	gdDocLine = dLine;

	if( !(gbExtract) )	ViewBrushFilling(  );	//	�u���V����
	//	���o���[�h���͏������Ȃ�

	gdXmemory = gdDocXdot;
	ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�ʒu�ړ�

	ViewSelMoveCheck( FALSE );

	ViewSelPositionSet( NULL );	//	�ړ������ʒu���L�^

	return;
}
//-------------------------------------------------------------------------------------------------

/*!
	�r���[�Ń}�E�X�𓮂������Ƃ�
	@param[in]	hWnd		�E�C���h�E�n���h���E�����r���[�̂��
	@param[in]	x			���������w���W�l
	@param[in]	y			���������x���W�l
	@param[in]	keyFlags	���ɉ�����Ă�L�[�ɂ���
*/
VOID Evw_OnMouseMove( HWND hWnd, INT x, INT y, UINT keyFlags )
{
	TCHAR	atString[SUB_STRING];
	INT		dX, dY;
	INT		dDot, dMaxDot, dLine, iMaxLine;	//	

	dX = x;
	dY = y;
	ViewPositionTransform( &dX, &dY, 0 );	//	�����ŁA�h�L�������g�ʒu�ɕύX

	ViewCombiKeyCheck(  );	//	�͈͑I�����悤�Ƃ��Ă邩

	//	�}�C�i�X�̂Ƃ��̓��[���[���s�ԍ��G���A
	if( 0 > dY )	dY = 0;

	dLine = dY / LINE_HEIGHT;

	if( 0 > dX )
	{
		dX = 0;
		if( (keyFlags & MK_LBUTTON) )	//	�h���b�O���ł���Ȃ�
		{
			//	���̍s�̃h�b�g�����m�F���āA��ɖ��[�ɃJ�[�\��������Ɖ��肷��
			dX = DocLineParamGet( dLine, NULL, NULL );
		}
	}

	dDot  = dX;

	gstCursor.x = dDot;
	gstCursor.y = dLine;	//	Cursor�ʒu�L��

	if( (keyFlags & MK_LBUTTON) )
	{
		//	�s���m�F���āA�͂ݏo���Ă��疖�[�ɂ��Ă���
		iMaxLine = DocPageParamGet( NULL, NULL );
		if( iMaxLine <= dLine ){	dLine = iMaxLine - 1;	}

		//	���̍s�̃h�b�g�����m�F���āA�͂ݏo���Ă��疖�[�ɂ��Ă���
		dMaxDot = DocLineParamGet( dLine, NULL, NULL );
		if( dMaxDot <=dDot )	dDot = dMaxDot;

		//	�����ʒu�ɍ��킹�Ē���
		gdDocMozi = DocLetterPosGetAdjust( &dDot, dLine, 0 );
		gdDocXdot = dDot;
		gdDocLine = dLine;

		ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�ʒu�ړ�

		ViewSelMoveCheck( TRUE );	//	�h���b�O�ړ��͑I�����[�h�˓�

		ViewSelPositionSet( NULL );	//	�ړ������ʒu���L�^
	}

	StringCchPrintf( atString, SUB_STRING, TEXT("MOUSE %d[dot] %d[line]"), gstCursor.x, gstCursor.y );
	StatusBarSetText( SB_MOUSEPOS, atString );

	return;
}
//-------------------------------------------------------------------------------------------------

/*!
	�r���[�Ń}�E�X�̍��{�^�������������ꂽ�Ƃ�
	@param[in]	hWnd			�E�C���h�E�n���h���E�r���[�̂Ƃ͌���Ȃ��̂Œ��ӃZ��
	@param[in]	x				���������w���W�l
	@param[in]	y				���������x���W�l
	@param[in]	keyFlags		���ɉ�����Ă�L�[�ɂ���
*/
VOID Evw_OnLButtonUp( HWND hWnd, INT x, INT y, UINT keyFlags )
{
	ViewSelRangeCheck( FALSE  );	//	�Ƃ肠�����I��͈̗͂l�q�m�F

	ReleaseCapture(   );	//	�}�E�X�L���v�`��

	return;
}
//-------------------------------------------------------------------------------------------------

/*!
	�r���[�Ń}�E�X�̉E�{�^�����_�E�����ꂽ�Ƃ��E�R���e�L�X�g���j���[�̑O
	@param[in]	hWnd			�E�C���h�E�n���h���E�r���[�̂Ƃ͌���Ȃ��̂Œ��ӃZ��
	@param[in]	fDoubleClick	��O�_�u���N���b�N���ꂽ�ꍇ
	@param[in]	x				���������w���W�l
	@param[in]	y				���������x���W�l
	@param[in]	keyFlags		���ɉ�����Ă�L�[�ɂ���
*/
VOID Evw_OnRButtonDown( HWND hWnd, BOOL fDoubleClick, INT x, INT y, UINT keyFlags )
{
	//	�L�����b�g�ړ��̂ݖʓ|����
	//	20110704	�I�𒆂̓L�����b�g�ړ����Ȃ��悤�ɂ���

	INT		dX, dY;	//	
	INT		dDot, dMaxDot, dLine, iMaxLine;	//	

	SetFocus( hWnd );	//	�}�E�X�C���Ńt�H�[�J�X

	if( IsSelecting( NULL ) )	//	�I����ƒ��ł���Ȃ�Ȃɂ����Ȃ�
	{
		TRACE( TEXT("[%X]�}�E�X�E�_�E���@%d:%d�@�I��"), hWnd, x, y );
		return;
	}

	dX = x;
	dY = y;
	ViewPositionTransform( &dX, &dY, 0 );	//	�����ŁA�h�L�������g�ʒu�ɕύX

	dDot  = dX;
	dLine = dY / LINE_HEIGHT;

	TRACE( TEXT("[%X]�}�E�X�E�_�E��[%d:%d[%d] / %d:%d:%d]"), hWnd, dX, dY, dLine, gbShiftOn, gbCtrlOn, gbAltOn );


	if( 0 <= dX || 0 <= dY )	//	�}�C�i�X�̂Ƃ��̓��[���[���s�ԍ��G���A
	{
		//	�����ɂ��Ă�����

		//	�L���Ȉʒu���ǂ����m�F

		//	�s���m�F���āA�͂ݏo���Ă��疖�[�ɂ��Ă���
		iMaxLine = DocPageParamGet( NULL, NULL );
		if( iMaxLine <=dLine )	dLine = iMaxLine - 1;

		//	���̍s�̃h�b�g�����m�F���āA�͂ݏo���Ă��疖�[�ɂ��Ă���
		dMaxDot = DocLineParamGet( dLine, NULL, NULL );
		if( dMaxDot <=dDot )	dDot = dMaxDot;

		//	�����ʒu�ɍ��킹�Ē���
		gdDocMozi = DocLetterPosGetAdjust( &dDot, dLine, 0 );
		gdDocXdot = dDot;
		gdDocLine = dLine;

		ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�ʒu�ړ�

		ViewSelMoveCheck( FALSE );

		ViewSelPositionSet( NULL );	//	�ړ������ʒu���L�^

	}

	return;
}
//-------------------------------------------------------------------------------------------------

/*!
	�z�C�[�����]
	@param[in]	hWnd	�E�C���h�E�n���h��
	@param[in]	xPos	�X�N���[���w���W
	@param[in]	yPos	�X�N���[���x���W
	@param[in]	zDelta	��]�ʁE�P�Q�O�P�ʁEWHEEL_DELTA
	@param[in]	fwKeys	���ɉ�����Ă����L�[
*/
VOID Evw_OnMouseWheel( HWND hWnd, INT xPos, INT yPos, INT zDelta, UINT fwKeys )
{
	UINT	dCode;

	HWND	hChdWnd;
	POINT	stPoint;

	TRACE( TEXT("POS[%d x %d] DELTA[%d] K[%X]"), xPos, yPos, zDelta, fwKeys );
	//	fwKeys	SHIFT 0x4, CTRL 0x8

	stPoint.x = xPos;
	stPoint.y = yPos;
	ScreenToClient( ghPrntWnd, &stPoint );
	hChdWnd = ChildWindowFromPointEx( ghPrntWnd, stPoint, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT );


	if( ghViewWnd != hChdWnd )	return;

	if( 0 < zDelta )		dCode = SB_LINEUP;
	else if( 0 > zDelta )	dCode = SB_LINEDOWN;
	else					dCode = SB_ENDSCROLL;

	//	pos���A�z�C�[���t���O�ɂ���΂���
	FORWARD_WM_VSCROLL( ghViewWnd, NULL, dCode, 1, PostMessage );

	return;
}
//-------------------------------------------------------------------------------------------------

/*!
	���j�R�[�h�󔒂̑}��
	@param[in]	dCommando	���
	@return	INT	������
*/
INT ViewInsertUniSpace( UINT dCommando )
{
	INT	width;
	TCHAR	ch;

	TRACE( TEXT("�}���F���j�R�[�h��") );

	switch( dCommando )
	{
		case IDM_IN_01SPACE:	ch  = (TCHAR)0x200A;	break;	//	8202
		case IDM_IN_02SPACE:	ch  = (TCHAR)0x2009;	break;	//	8201
		case IDM_IN_03SPACE:	ch  = (TCHAR)0x2006;	break;	//	8198
		case IDM_IN_04SPACE:	ch  = (TCHAR)0x2005;	break;	//	8197
		case IDM_IN_05SPACE:	ch  = (TCHAR)0x2004;	break;	//	8196
		case IDM_IN_08SPACE:	ch  = (TCHAR)0x2002;	break;	//	8194
		case IDM_IN_10SPACE:	ch  = (TCHAR)0x2007;	break;	//	8199
		case IDM_IN_16SPACE:	ch  = (TCHAR)0x2003;	break;	//	8195
		default:	return 0;
	}

	width = DocInsertLetter( &gdDocXdot, gdDocLine, ch );	//	�s�ɒǉ�
	//	���ŃA���h�D�o�b�t�@�����O
	gdDocMozi = DocLetterPosGetAdjust( &gdDocXdot, gdDocLine, 0 );
	ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�ʒu�ړ�
	ViewRedrawSetLine( gdDocLine  );	//	�r���[�̏����������K�v

	DocPageInfoRenew( -1, 1 );

	return width;
}
//-------------------------------------------------------------------------------------------------

/*!
	������Ηp�F�w��^�O�̑}��
	@param[in]	dCommando	���
	@return	INT	������
*/
INT ViewInsertColourTag( UINT dCommando )
{
	UINT	dCrLf;
	INT		dDot;
	TCHAR	atString[MAX_STRING];

	switch( dCommando )
	{
		case IDM_INSTAG_WHITE:	StringCchCopy( atString, MAX_STRING, COLOUR_TAG_WHITE );	break;
		case IDM_INSTAG_BLUE:	StringCchCopy( atString, MAX_STRING, COLOUR_TAG_BLUE  );	break;
		case IDM_INSTAG_BLACK:	StringCchCopy( atString, MAX_STRING, COLOUR_TAG_BLACK );	break;
		case IDM_INSTAG_RED:	StringCchCopy( atString, MAX_STRING, COLOUR_TAG_RED   );	break;
		case IDM_INSTAG_GREEN:	StringCchCopy( atString, MAX_STRING, COLOUR_TAG_GREEN );	break;
		default:	return 0;
	}

	dDot = gdDocXdot;

	dCrLf = DocInsertString( &gdDocXdot, &gdDocLine, &gdDocMozi, atString, 0, TRUE );

	dDot = gdDocXdot- dDot;

	DocPageInfoRenew( -1, 1 );

	return dDot;
}
//-------------------------------------------------------------------------------------------------

/*!
	�e���v������̕������}��
	@param[in]	ptText	�}�����镶����
	@return		�g�����h�b�g��
*/
INT ViewInsertTmpleString( LPTSTR ptText )
{
	UINT	dCrLf;
	INT		dDot;

	dDot = gdDocXdot;

	dCrLf = DocInsertString( &gdDocXdot, &gdDocLine, &gdDocMozi, ptText, 0, TRUE );

	dDot = gdDocXdot- dDot;

	DocPageInfoRenew( -1, 1 );

	return dDot;
}
//-------------------------------------------------------------------------------------------------

/*!
	�u���V���[�h�̐ݒ��������
	@param[in]	bBrushOn	�u���V���[�h���ۂ�
	@param[in]	ptPattern	�g�p����u���V�p���[��
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT ViewBrushStyleSetting( UINT bBrushOn, LPTSTR ptPattern )
{
	gbBrushMode = bBrushOn;

	//	���j���[��checkTOGGLE
	SendMessage( ghPrntWnd, WMP_BRUSH_TOGGLE, (WPARAM)bBrushOn, (LPARAM)IDM_BRUSH_STYLE );

	if( ptPattern ){	StringCchCopy( gatBrushPtn, SUB_STRING, ptPattern );	}

	OperationOnStatusBar(  );

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�u���V���[�h�Ȃ�A�u���V�I����������
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT ViewBrushFilling( VOID )
{
	INT			dTgDot;
	INT			dLeft, dRight, iBgnMozi, iCntMozi, rslt;
	BOOLEAN		bSpace, bFirst = TRUE;
	LPTSTR		ptBuff;
	wstring		wsBuff;


	if( !(gbBrushMode) )	return S_FALSE;

	//	�I��͈͂�����Ȃ�A��������D�悵�ēh��Ԃ�
	rslt = DocSelectedBrushFilling( gatBrushPtn, &gdDocXdot, &gdDocLine );
	if( rslt )	return S_OK;

	//	�N���b�R�ʒu���󔒂��ǂ���
	dTgDot = DocLineStateCheckWithDot( gdDocXdot, gdDocLine, &dLeft, &dRight, &iBgnMozi, &iCntMozi, &bSpace );
	if( !(bSpace)  )	return S_FALSE;

//�h��ׂ�������쐬
	ptBuff = BrushStringMake( dTgDot, gatBrushPtn );
	if( !(ptBuff) )
	{
		NotifyBalloonExist( NULL, TEXT("�u���V��I��ł����ė~�����̂ł��B��������"), TEXT("��������"), NIIF_INFO );
		return E_OUTOFMEMORY;
	}

	//	���͈̔͂̋󔒂��폜����
	DocRangeDeleteByMozi( dLeft, gdDocLine, iBgnMozi, (iBgnMozi + iCntMozi), &bFirst );
	//	�u���V��������Ԃ�����
	DocInsertString( &dLeft, &gdDocLine, NULL, ptBuff, 0, bFirst );	bFirst = FALSE;

	//	�ēx�����E�L�����b�g�ړ��͂��̌�
	gdDocMozi = DocLetterPosGetAdjust( &gdDocXdot, gdDocLine, 0 );

	FREE(ptBuff);

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��̕��ƃp�^�[���ŁA�u���V�镶��������B�]��͓K���ɖ��߂�E����Ă��C�ɂ��Ȃ�
	@param[in]	dDotLen		�쐬����h�b�g��
	@param[in]	ptPattern	�g�p����u���V�p���[��
	@return		LPTSTR	�쐬����������E�J���͌Ă񂾕��Ŗʓ|����E�쐬�o���Ȃ�������NULL
*/
LPTSTR BrushStringMake( INT dDotLen, LPTSTR ptPattern )
{
	INT			dPtnDot, dCnt, dAmr, i, wid;
#if 0
	INT			chk;
#endif
	UINT_PTR	cchSize;
	LPTSTR		ptBuff;//, ptPadd = NULL;
	wstring		wsBuff;

//�h��ׂ�������쐬
	wsBuff.clear( );

	//	�u���V�̕��m�F����
	dPtnDot = ViewStringWidthGet( ptPattern );
	if( 0 >= dPtnDot ||  0 >= dDotLen ){	return NULL;	}
	//	�Ȃ񂩂��낢�남������������I���

	dCnt = dDotLen / dPtnDot;	//	�K�v�Ȑ����m�F
	dAmr = dDotLen - (dCnt * dPtnDot);	//	�]��

	//	�K��̕�����쐬
	for( i = 0; dCnt > i; i++ ){	wsBuff += ptPattern;	}

	//	���܂���o������薄�߂�
	i = 0;
	while( 0 < dAmr )	//	�]�蕝�������Ȃ�����I���
	{
		if(  0 == ptPattern[i] )	break;	//	����ȏ�p���[�����Ȃ�������

		wid = ViewLetterWidthGet( ptPattern[i] );	//	���ԂɌ��Ă���
#if 0
		if( gbUniPad )	//	���j�R�[�h�󔒎g���Ȃ�A�͂ݏo�����O�ŏI����Ă���
		{
			chk = dAmr - wid;
			if( 0 > chk )	break;
		}
#endif
		wsBuff += ptPattern[i];
		dAmr -= wid;	//	�땶�����̕��Ŗ��߂Ă���
		i++;
	}

	cchSize = wsBuff.size( ) + 8;	//	�Ƃ肠�����]�T���Ă���
	ptBuff = (LPTSTR)malloc( cchSize * sizeof(TCHAR) );
	ZeroMemory( ptBuff, cchSize * sizeof(TCHAR) );
	StringCchCopy( ptBuff, cchSize, wsBuff.c_str( ) );
#if 0
	if( gbUniPad )	//	���j�R�[�h�󔒂ŁA�]�������h�b�g�𖄂߂Ă���
	{
		ptPadd = DocPaddingSpaceUni( dAmr, NULL, NULL, NULL );
		if( ptPadd )
		{
			StringCchCat( ptBuff, cchSize, ptPadd );
			FREE(ptPadd);
		}
	}
#endif

	return ptBuff;
}
//-------------------------------------------------------------------------------------------------

/*!
	WM_IME_COMPOSITION���b�Z�[�W
	@param[in]	hWnd		�E�C���h�E�n���h��
	@param[in]	wParam		�ǉ��̏��P
	@param[in]	lParam		�ǉ��̏��Q
*/
VOID Evw_OnImeComposition( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	HIMC	hImc;
	LONG	cbSize;
	LPTSTR	ptBuffer;
	UINT_PTR	cchSize;

	BOOLEAN	bSelect = FALSE;
	UINT	bSqSel  = 0;

	TRACE( TEXT("WM_IME_COMPOSITION[0x%X][0x%X]"), wParam, lParam );

	bSelect = IsSelecting( &bSqSel );
	gdSqFillCnt = 0;

	if( (GCS_RESULTSTR & lParam) && bSelect && bSqSel )
	{
		hImc = ImmGetContext( ghViewWnd );	//	IME�n���h���m��

		cbSize = ImmGetCompositionString( hImc, GCS_RESULTSTR, NULL, 0 );
		//	������̂̓o�C�g���E����������Ȃ�

		//	�m�肵����������m��
		cbSize += 2;
		ptBuffer = (LPTSTR)malloc( cbSize );
		ZeroMemory( ptBuffer, cbSize );
		ImmGetCompositionString( hImc, GCS_RESULTSTR, ptBuffer, cbSize );
		TRACE( TEXT("COMPOSITION MOZI[%d][%s]"), cbSize, ptBuffer );
		ImmReleaseContext( ghViewWnd , hImc );

		//	�h��ׂ�����
		DocSelectedBrushFilling( ptBuffer, &gdDocXdot ,&gdDocLine );
		ViewDrawCaret( gdDocXdot, gdDocLine, 1 );	//	�L�����b�g�ʒu�����߂�
		DocPageInfoRenew( -1, 1 );

		StringCchLength( ptBuffer, cbSize, &cchSize );
		gdSqFillCnt = cchSize;

		FREE(ptBuffer);
	}
/*
GCS_COMPREADSTR			0x0001	���݂̕ҏW�̃��[�f�B���O�����񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_COMPREADATTR		0x0002	���݂̕ҏW�̃��[�f�B���O�����񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_COMPREADCLAUSE		0x0004	�ҏW������̃��[�f�B���O������̕��ߏ�񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_COMPSTR				0x0008	���݂̕ҏW�����񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_COMPATTR			0x0010	�ҏW������̃A�g���r���[�g���擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_COMPCLAUSE			0x0020	�ҏW������̕��ߏ�񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_CURSORPOS			0x0080	�ҏW������̃J�[�\���ʒu���擾�����A�܂��͍ŐV�����ꂽ
GCS_DELTASTART			0x0100	�ҏW������̕ω��̊J�n�ʒu���擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_RESULTREADSTR		0x0200	���[�f�B���O��������擾�����A�܂��͍ŐV�����ꂽ
GCS_RESULTREADCLAUSE	0x0400	���[�f�B���O������̕��ߏ�񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_RESULTSTR			0x0800	�m�蕶���񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
GCS_RESULTCLAUSE		0x1000	�m�蕶����̕��ߏ�񂪎擾���ꂽ�A�܂��͍ŐV�����ꂽ
*/
	return;
}
//-------------------------------------------------------------------------------------------------
