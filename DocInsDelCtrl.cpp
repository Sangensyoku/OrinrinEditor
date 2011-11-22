/*! @file
	@brief �����̒ǉ��폜�n�̖ʓ|�݂܂�
	���̃t�@�C���� DocInsDelCtrl.cpp �ł��B
	@author	SikigamiHNQ
	@date	2011/04/24
*/

#include "stdafx.h"
#include "OrinrinEditor.h"
//-------------------------------------------------------------------------------------------------

typedef struct tagPAGENUMINFO
{
	UINT	dStartNum;	//	�J�n�ԍ�
	UINT	bInUnder;	//	��O�ŉ����@�O�Ő擪
	UINT	bOverride;	//	�s���e�ɏ㏑
	TCHAR	atStyle[MAX_PATH];	//	����

} PAGENUMINFO, *LPPAGENUMINFO;
//-------------------------------------------------------------------------------------------------

#ifdef MULTI_FILE
extern FILES_ITR	gitFileIt;	//	�����Ă�t�@�C���̖{��
#define gstFile	(*gitFileIt)	//!<	�C�e���[�^���\���̂ƌ��Ȃ�
#else
extern ONEFILE	gstFile;		//	�t�@�C���P�ʁE�����t�@�C���ɂ͂ǂ�����đΉ����H
#endif
extern INT		gixFocusPage;	//	���ڒ��̃y�[�W�E�Ƃ肠�����O�E�O�C���f�b�N�X

extern  UINT	gbUniPad;		//	�p�f�B���O�Ƀ��j�R�[�h�������āA�h�b�g�������Ȃ��悤�ɂ���
extern  UINT	gbUniRadixHex;	//	���j�R�[�h���l�Q�Ƃ��P�U�i���ł��邩

extern  UINT	gdRightRuler;	//	�E���̈ʒu
//-------------------------------------------------------------------------------------------------

/*
�͈͑I�𒆂ɁAALT���E������A�����̕������X���C�h����Ƃ��B��s�I�𒆂̂݁H
*/


HRESULT	DocInputReturn( INT, INT );

INT		DocSquareAddPreMod( INT, INT, INT, BOOLEAN );

INT		DocLetterErase( INT, INT, INT );
//-------------------------------------------------------------------------------------------------


//���������R�[�h�́ADLL����g���悤�ɂ���΁A��萸�x�̗ǂ��̂Ƃ��������Ƃ� 
/*!
	�Y���̃��j�R�[�h�������A�V�t�gJIS�ɕϊ��o���邩�ǂ����m�F
	@param[in]	cchMozi	�m�F�����������R
	@param[out]	pcSjis	�ϊ��������ʂ�����o�b�t�@�ւ̃|�C���^�E�P�O�o�C�g�Ƃ邱��
	@return				��O�]���ł���@�O����
*/
BOOLEAN DocIsSjisTrance( TCHAR cchMozi, LPSTR pcSjis )
{
	TCHAR	atMozi[2];
	CHAR	acSjis[10];
	BOOL	bCant = FALSE;
	INT		iRslt;
/*
	�V�t�gJIS�ɂł��Ȃ����j�R�[�h�����ɂ���
	WideCharToMultiByte�ŁA�ϊ��s�����������ꍇ�̃t���O���m�F���āA
	�����́u&#dddd;�v�u&#xhhhh;�v�ɕϊ�����悤�ɂ���E
*/
	assert( pcSjis );

	atMozi[0] = cchMozi;	atMozi[1] = 0;
	acSjis[0] = 0;	acSjis[1] = 0;	acSjis[2] = 0;

	iRslt = WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, atMozi, 1, acSjis, 10, "?", &bCant );

	if( bCant )
	{
	//	TRACE( TEXT("SJIS�s��(%d:%X)"), cchMozi, cchMozi );
		if( gbUniRadixHex ){	StringCchPrintfA( acSjis, 10, ("&#x%X;"), cchMozi );	}
		else{					StringCchPrintfA( acSjis, 10, ("&#%d;"),  cchMozi );	}
	}

	StringCchCopyA( pcSjis, 10, acSjis );

	return bCant ? FALSE : TRUE;
}
//-------------------------------------------------------------------------------------------------

/*!
	���̃o�C�g�����m�F
	@param[in,out]	pstLet	�����f�[�^���ꂽ��o������
	@return	INT_PTR	�o�C�g��
*/
INT_PTR DocLetterByteCheck( LPLETTER pstLet )
{
	pstLet->mzByte = strlen( pstLet->acSjis );	//	�ʏ�́A�������̓��j�R�[�h�X�^�C��

	if( pstLet->mzStyle & CT_CANTSJIS ){	pstLet->mzByte += 4;	}	//	���l�Q�Ƃ̐擪�́��͂T�o�C�g�v�Z�K�v

	if( 1 == pstLet->mzByte )	//	�P�o�C�g�����ǎ��͈Ⴄ���c��T��
	{
		//	���p�J�^�J�i
		if( 0xA1 <= (BYTE)(pstLet->acSjis[0]) && (BYTE)(pstLet->acSjis[0]) <= 0xDF ){	pstLet->mzByte =  2;	}

		//	HTML����L��
		else if( '"' == pstLet->acSjis[0] ){	pstLet->mzByte = strlen( ("&quot;") );	}
		else if( '<' == pstLet->acSjis[0] ){	pstLet->mzByte = strlen( ("&lt;") );	}
		else if( '>' == pstLet->acSjis[0] ){	pstLet->mzByte = strlen( ("&gt;") );	}
		else if( '&' == pstLet->acSjis[0] ){	pstLet->mzByte = strlen( ("&amp;") );	}
	}

	return pstLet->mzByte;
}
//-------------------------------------------------------------------------------------------------

/*!
	������̉��s����������
	@param[in]	xDot	�L�����b�g�̃h�b�g�ʒu
	@param[in]	yLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in]	bFirst	�A���h�D�����̐擪���ǂ���
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT DocCrLfAdd( INT xDot, INT yLine, BOOLEAN bFirst )
{
	SqnAppendString( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_INSERT, CH_CRLFW, xDot, yLine, bFirst );

	return DocInputReturn( xDot , yLine );
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�ŉ��s����
	@param[in]	nowDot	���̃L�����b�g�̃h�b�g�ʒu
	@param[in]	rdLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT DocInputReturn( INT nowDot, INT rdLine )
{
	INT_PTR	iLetter, iLines, iCount;
	ONELINE	stLine;

	LINE_ITR	vcLineItr;
	LTR_ITR		vcLtrItr, vcLtrEnd;

	iLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );
	if( iLines <= rdLine )	return E_OUTOFMEMORY;

	ZeroONELINE( &stLine );

	//	���̕����ʒu���m�F�E���ݍs�Ȃ̂ł���Ă͂Ȃ��͂�
	iLetter = DocLetterPosGetAdjust( &nowDot, rdLine, 0 );

	//	�������m�F
	iCount = gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.size( );



	if( iLetter < iCount )	//	�����s�̓r���ŉ��s������H
	{

		vcLineItr = gstFile.vcCont.at( gixFocusPage ).vcPage.begin( );
		vcLineItr += (rdLine+1);
		//	���̍s�̎��̏ꏊ�ɍs�̃f�[�^��}��
		gstFile.vcCont.at( gixFocusPage ).vcPage.insert( vcLineItr, stLine );

		//	���̍s�́A�����f�[�^�̐擪���Ƃ�
		vcLineItr = gstFile.vcCont.at( gixFocusPage ).vcPage.begin( );
		vcLineItr += (rdLine+1);	//	�ǉ������s�܂ňړ�

		//	�Ԃ����؂����ꏊ��ݒ肵�Ȃ�����
		vcLtrItr  = gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.begin( );
		vcLtrItr += iLetter;	//	���̕����ʒu��������
		vcLtrEnd  = gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.end( );	//	���[

		//	���̕��������̍s�ɃR�s�[����
		std::copy( vcLtrItr, vcLtrEnd, back_inserter(vcLineItr->vcLine) );

		//	���̕�������폜����
		gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.erase( vcLtrItr, vcLtrEnd );

		//	���h�b�g���Čv�Z
		DocLineParamGet( rdLine,   NULL, NULL );
		DocLineParamGet( rdLine+1, NULL, NULL );
	}
	else	//	���[�ŉ��s����
	{
		if( (iLines - 1) == rdLine )	//	EOF�I�ȂƂ���
		{
			gstFile.vcCont.at( gixFocusPage ).vcPage.push_back( stLine );
		}
		else
		{
			vcLineItr = gstFile.vcCont.at( gixFocusPage ).vcPage.begin( );
			vcLineItr += (rdLine+1);	//	���̍s��������

			//	���̏ꏊ�ɍs�̃f�[�^��}��
			gstFile.vcCont.at( gixFocusPage ).vcPage.insert( vcLineItr, stLine );
		}
	}

	DocBadSpaceCheck( rdLine   );	//	�����ŋ󔒃`�F�L
	DocBadSpaceCheck( rdLine+1 );	//	�󔒃`�F�L�E���̍s���m�F

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�Ńo�b�N�X�y�[�X������
	@param[in]	pdDot	���̃L�����b�g�̃h�b�g�ʒu�̒l�ւ̃|�C���^�[
	@param[in]	pdLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@return	INT	��O���s�������@�O��s�̂�
*/
INT DocInputBkSpace( PINT pdDot, PINT pdLine )
{
	INT_PTR	iLines;
	INT		iLetter, width = 0, neDot, bCrLf = 0;
	INT		dLine = *pdLine;	//	�������Ŏg���s�ԍ��E�����ɒ���
	TCHAR	ch;


	//	�͂ݏo���Ă���A�E�c�I
	iLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );
	if( iLines <=  dLine )	return 0;

	//	���̕����ʒu���m�F�E���ݍs�Ȃ̂ł���Ă͂Ȃ��͂�
	iLetter = DocLetterPosGetAdjust( pdDot, dLine, 0 );
	neDot = *pdDot;

//	TRACE( TEXT("���[D%d C%d]"), neDot, iLetter );

	if( 0 == iLetter && 0 == dLine )	return 0;	//	�擪���ŏ��̍s�Ȃ�A�Ȃɂ����Ȃ�

	if( 0 != iLetter )	//	�s�̐擪�łȂ��Ȃ�
	{
		iLetter--;	//	�L�����b�g��߂�
		width = gstFile.vcCont.at( gixFocusPage ).vcPage.at( dLine ).vcLine.at( iLetter ).rdWidth;
		ch    = gstFile.vcCont.at( gixFocusPage ).vcPage.at( dLine ).vcLine.at( iLetter ).cchMozi;
		*pdDot = neDot - width;	//	���������h�b�g���߂���
		bCrLf = 0;

		SqnAppendLetter( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_DELETE, ch, *pdDot, dLine, TRUE );
	}
	else	//	�s�̐擪�ł���Ȃ�
	{
		dLine--;	*pdLine = dLine;	//	�O�̍s�Ɉړ�����
		neDot = DocLineParamGet( dLine, &iLetter, NULL );
		*pdDot = neDot;	//	CARET�ʒu����
		bCrLf = 1;

		//	�����ł���Ė�薳���͂�
		SqnAppendString( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_DELETE, CH_CRLFW, *pdDot, dLine, TRUE );
	}

	DocLetterErase( *pdDot, dLine, iLetter );

	return bCrLf;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�Ńf���[�g������
	@param[in]	xDot	���̃L�����b�g�̃h�b�g�ʒu
	@param[in]	yLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@return	INT			��O���s�폜�E�O�����폜
*/
INT DocInputDelete( INT xDot, INT yLine )
{
	INT_PTR	iLines;
	INT		iCount, iLetter, iCrLf;
	TCHAR	ch;


	//	�͂ݏo���Ă���A�E�c�I
	iLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );
	if( iLines <= yLine )	return 0;

	//	���̕����ʒu���m�F�E���ݍs�Ȃ̂ł���Ă͂Ȃ��͂�
	iLetter = DocLetterPosGetAdjust( &xDot, yLine, 0 );

//	TRACE( TEXT("�폜[D%d C%d]"), xDot, iLetter );

	DocLineParamGet( yLine, &iCount, NULL );	//	���̍s�̕�������l��

	if( iCount <= iLetter )
	{
		if( iLines <= (yLine+1) )	return 0;	//	���S�ɖ��[�Ȃ牽�����Ȃ�
		ch = CC_LF;
	}
	else
	{
		ch = gstFile.vcCont.at( gixFocusPage ).vcPage.at( yLine ).vcLine.at( iLetter ).cchMozi;
	}

	iCrLf = DocLetterErase( xDot, yLine, iLetter );

	if( iCrLf )
	{
		SqnAppendString( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_DELETE, CH_CRLFW, xDot, yLine, TRUE );
	}
	else
	{
		SqnAppendLetter( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_DELETE, ch, xDot, yLine, TRUE );
	}

	return iCrLf;
}
//-------------------------------------------------------------------------------------------------



/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�ň땶���폜
	@param[in]	xDot	���̃L�����b�g�̃h�b�g�ʒu�̒l
	@param[in]	yLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in]	iLetter	�Ώۂ̕����ʒu
	@return	INT			��O���s�폜�E�O�����폜
*/
INT DocLetterErase( INT xDot, INT yLine, INT iLetter )
{
	INT		iCount;

	LTR_ITR	vcLtrItr;

	DocLineParamGet( yLine, &iCount, NULL );	//	���̍s�̕�������l��

	//	��������DELETE�̏���
	if( iCount > iLetter )	//	���[�łȂ��Ȃ�A���̕���������OK
	{
		vcLtrItr  = gstFile.vcCont.at( gixFocusPage ).vcPage.at( yLine ).vcLine.begin( );
		vcLtrItr += iLetter;	//	���̕����ʒu��������

		DocIterateDelete( vcLtrItr, yLine );
		return 0;
	}
	else
	{
		DocLineCombine( yLine );
		return 1;
	}

//	return 0;	//	���䂪�n��Ȃ��R�[�h
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̓��e���폜����E���s�͂��̂܂�
	@param[in]	yLine	�Ώۂ̍s�ԍ�
	@param[in]	bFirst	�A���h�D�̔�O���߂ẴO���[�v�@�O�����̏���
	@return		�A���h�D�󋵁F�폜���Ă���FALSE�A�i�j�����Ȃ�������󂯂��̃X���[
*/
BOOLEAN DocLineErase( INT yLine, BOOLEAN bFirst )
{
	INT		dLines, iMozis, i;
	INT_PTR	cbSize, cchSize;
	LPTSTR	ptBuffer;
	wstring	wsString;

	wsString.clear( );

	dLines = DocPageParamGet( NULL, NULL );
	if( dLines <= yLine )	return bFirst;	//	�͂ݏo���m�F

	DocLineParamGet( yLine, &iMozis, NULL );	//	�w��s�̕������m��

	if( 0 >= iMozis )	return bFirst;	//	�������Ȃ��Ȃ炷�邱�Ɩ���

	for( i = 0; iMozis > i; i++ )	//	�S�������m��
	{
		wsString += gstFile.vcCont.at( gixFocusPage ).vcPage.at( yLine ).vcLine.at( i ).cchMozi;
	}

	cchSize = wsString.size(  ) + 1;	//	NULL�^�[�~�l�[�^������
	cbSize = cchSize * 2;	//	���j�R�[�h�Ȃ̂Ńo�C�g���͂Q�{�ł���

	ptBuffer = (LPTSTR)malloc( cbSize );
	ZeroMemory( ptBuffer, cbSize );
	StringCchCopy( ptBuffer, cchSize, wsString.c_str(  ) );
	SqnAppendString( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_DELETE, ptBuffer, 0, yLine, bFirst );
	bFirst = FALSE;

	//	�폜����
	gstFile.vcCont.at( gixFocusPage ).vcPage.at( yLine ).vcLine.clear();

	DocLineParamGet( yLine, NULL, NULL );	//	�Čv�Z
	DocPageParamGet( NULL, NULL );	//	�Čv�Z
	DocBadSpaceCheck( yLine );	//	���Z�b�g�ɕK�v
	ViewRedrawSetLine( yLine );	//	�v��Ȃ�����

	return bFirst;
}
//-------------------------------------------------------------------------------------------------

/*!
	�Ώە����̃C�e���[�^�ƍs���󂯂āA���̕������폜����
	@param[in]	itLtr	�Ώە����̃C�e���[�^
	@param[in]	dBsLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@return	INT			��O���s�폜�E�O�����폜
*/
INT DocIterateDelete( LTR_ITR itLtr, INT dBsLine )
{
	INT	width = 0, bySz;

	width = itLtr->rdWidth;
	bySz  = itLtr->mzByte;

	gstFile.vcCont.at( gixFocusPage ).vcPage.at( dBsLine ).vcLine.erase( itLtr );

	gstFile.vcCont.at( gixFocusPage ).vcPage.at( dBsLine ).iDotCnt -= width;
	gstFile.vcCont.at( gixFocusPage ).vcPage.at( dBsLine ).iByteSz -= bySz;

	gstFile.vcCont.at( gixFocusPage ).dByteSz -= bySz;

	DocBadSpaceCheck( dBsLine );	//	���łɗǂ��Ȃ��X�y�[�X�𒲂ׂĂ���

	return width;
}
//-------------------------------------------------------------------------------------------------

/*!
	�Ώۍs�́A���̍s���A�Ώۍs�̖����ɂ�������B���[��DELETE����
	@param[in]	dBsLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X
	@return		���ɈӖ��͂Ȃ�
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT DocLineCombine( INT dBsLine )
{
	LTR_ITR		vcLtrItr, vcLtrNxItr, vcLtrNxEnd;
	LINE_ITR	vcLineItr;
//	INT	iRslt;

	vcLtrNxItr = gstFile.vcCont.at( gixFocusPage ).vcPage.at( dBsLine+1 ).vcLine.begin(  );	//	���̍s�̐擪
	vcLtrNxEnd = gstFile.vcCont.at( gixFocusPage ).vcPage.at( dBsLine+1 ).vcLine.end(  );	//	���̍s�̐K��

	std::copy( vcLtrNxItr, vcLtrNxEnd, back_inserter( gstFile.vcCont.at( gixFocusPage ).vcPage.at( dBsLine ).vcLine ) );

	DocLineParamGet( dBsLine , NULL, NULL );	//	�Ăяo���Β��Ŗʓ|�݂Ă����

	vcLineItr  = gstFile.vcCont.at( gixFocusPage ).vcPage.begin(  );
	vcLineItr += (dBsLine+1);	//	���̍s

	gstFile.vcCont.at( gixFocusPage ).vcPage.erase( vcLineItr );

	DocBadSpaceCheck( dBsLine );	//	���łɗǂ��Ȃ��X�y�[�X�𒲂ׂĂ���

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�Ɉ땶���ǉ����ăA���h�D�L�^����
	@param[in]	pxDot	�}������h�b�g�ʒu�ւ̃|�C���^�[
	@param[in]	yLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in]	ch		�ǉ�����������
	@return		INT		�ǉ����������̃h�b�g��
*/
INT DocInsertLetter( PINT pxDot, INT yLine, TCHAR ch )
{
	INT	width;

	SqnAppendLetter( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_INSERT, ch, *pxDot, yLine, TRUE );

	width = DocInputLetter( *pxDot, yLine, ch );
	*pxDot += width;	//	�r���ł�������

	DocBadSpaceCheck( yLine );	//	�����ŋ󔒃`�F�L

	return width;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�Ɉ땶���ǉ�����E���̔������ł̓A���h�D�̖ʓ|�͌��Ȃ�
	@param[in]	nowDot	�}������h�b�g�ʒu
	@param[in]	rdLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in]	ch		�ǉ�����������
	@return		INT		�ǉ����������̃h�b�g��
*/
INT DocInputLetter( INT nowDot, INT rdLine, TCHAR ch )
{
	INT_PTR	iLetter, iCount, iLines;
	LETTER	stLetter;
	LTR_ITR	vcItr;

	//	�A���h�D���h�D�͌Ă񂾂Ƃ����

	iLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );
	if( iLines <=  rdLine )	return 0;

	//	���̕����ʒu���m�F�E���ݍs�Ȃ̂ł���Ă͂Ȃ��͂�
	iLetter = DocLetterPosGetAdjust( &nowDot, rdLine, 0 );

	//	�������m�F
	iCount = gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.size( );

	//	�f�[�^�쐬
	ZeroMemory( &stLetter, sizeof(LETTER) );
	stLetter.cchMozi = ch;
	stLetter.rdWidth = ViewLetterWidthGet( ch );
	stLetter.mzStyle = CT_NORMAL;
	if( iswspace( ch ) )	stLetter.mzStyle |= CT_SPACE;
	//	��V�t�gJIS�������m�F
	if( !( DocIsSjisTrance(ch,stLetter.acSjis) ) )	stLetter.mzStyle |= CT_CANTSJIS;
	//	�����̃o�C�g�T�C�Y�m�F
	DocLetterByteCheck( &stLetter );
	//stLetter.mzByte = strlen( stLetter.acSjis );	//	�ʏ�́A�������̓��j�R�[�h�X�^�C��
	//if( stLetter.mzStyle & CT_CANTSJIS ){	stLetter.mzByte += 4;	}	//	���l�Q�Ƃ̐擪�́��͂T�o�C�g�v�Z�K�v
	//if( 1 == stLetter.mzByte )	//	�P�o�C�g�����ǎ��͈Ⴄ���c��T��
	//{
	//	if( 0xA1 <= (BYTE)(stLetter.acSjis[0]) && (BYTE)(stLetter.acSjis[0]) <= 0xDF ){	stLetter.mzByte = 2;	}	//	���p�J�^�J�i
	//	else if( '"' == stLetter.acSjis[0] ){	stLetter.mzByte  = strlen( ("&quot;") );	}	//	HTML����L��
	//	else if( '<' == stLetter.acSjis[0] ){	stLetter.mzByte = strlen( ("&lt;") );	}	//	HTML����L��
	//	else if( '>' == stLetter.acSjis[0] ){	stLetter.mzByte = strlen( ("&gt;") );	}	//	HTML����L��
	//	else if( '&' == stLetter.acSjis[0] ){	stLetter.mzByte = strlen( ("&amp;") );	}	//	HTML����L��
	//}



	if( iLetter >=  iCount )	//	�����������Ȃ疖�[�ɒǉ��Ƃ�������
	{
		gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.push_back( stLetter );
	}
	else	//	�����łȂ��Ȃ�r���ɒǉ�
	{
		vcItr = gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.begin( );
		vcItr += iLetter;
		gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).vcLine.insert( vcItr, stLetter );
	}

	gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).iDotCnt += stLetter.rdWidth;
	gstFile.vcCont.at( gixFocusPage ).vcPage.at( rdLine ).iByteSz += stLetter.mzByte;

	gstFile.vcCont.at( gixFocusPage ).dByteSz += stLetter.mzByte;

//	DocBadSpaceCheck( rdLine );	�Ă񂾂Ƃ���ł܂Ƃ߂Ă��

	return stLetter.rdWidth;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu�ŕ��������폜����E���s�͓󕶎���L
	@param[in]	xDot	���̃L�����b�g�̃h�b�g�ʒu
	@param[in]	yLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in]	ptDummy	���g�p�ENULL��
	@param[in]	cchSize	������̕������E�k���^�[�~�l�[�^�܂܂�
	@return		INT		�O���s�����@�P�`���s���T�N������
*/
INT DocStringErase( INT xDot, INT yLine, LPTSTR ptDummy, INT cchSize )
{
	INT	i, dCrLf, iLetter, rdCnt;

	//	���̕����ʒu�E�L�����b�g��薖�������ɍ폜����̂ŁA���̈ʒu�͕ς��Ȃ�
	iLetter = DocLetterPosGetAdjust( &xDot, yLine, 0 );

	rdCnt = 0;
	for( i = 0; cchSize > i; i++ )	//	DEL�A�ł��Ă���
	{
		dCrLf = DocLetterErase( xDot, yLine, iLetter );
		if( dCrLf ){	i++;	rdCnt++;	}
	}

	return rdCnt;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�ɕ������ǉ�����E�����������ʔ���
	@param[in]	pNowDot	���̃L�����b�g�̃h�b�g�ʒu�̃|�C���^�[
	@param[in]	pdLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in]	ptStr	�ǉ�������������
	@param[in]	cchSize	������̕������E�k���^�[�~�l�[�^�܂܂�
	@return		INT		�O���s�����@�P�`���s������
*/
INT DocStringAdd( PINT pNowDot, PINT pdLine, LPTSTR ptStr, INT cchSize )
{
	INT	i, insDot, dLn, dCrLf;

	assert( ptStr );

	dCrLf  = 0;
	dLn    = *pdLine;
	insDot = *pNowDot;

	for( i = 0; cchSize > i; i++ )
	{
		if( CC_CR == ptStr[i] && CC_LF == ptStr[i+1] )	//	���s�ł�������
		{
			DocInputReturn( insDot, *pdLine );
			i++;			//	0x0D,0x0A������A�땶����΂��̂��|�C���g
			(*pdLine)++;	//	���s��������Focus�͎��̍s��
			insDot =  0;	//	�����čs�̐擪�ł���
			dCrLf++;		//	���s�����񐔃J�E���g
		}
		else if( CC_TAB == ptStr[i] )
		{
			//	�^�u�͑}�����Ȃ�
		}
		else
		{
			insDot += DocInputLetter( insDot, *pdLine, ptStr[i] );
		}
	}

	//	�����ŋ󔒃`�F�L�E�J�n�s����I���s�܂Ńu���u������
	for( i = dLn; *pdLine >= i; i++ )
	{
		DocBadSpaceCheck( i );
	}


	//	�A���h�D���h�D�͂����ł͂Ȃ��Ă񂾂ق��Ŗʓ|����ق�������

	*pNowDot = insDot;

	return dCrLf;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̃h�b�g�ʒu(�L�����b�g�ʒu)�ɕ��������`�Œǉ�����
	@param[in]	pNowDot	���̃L�����b�g�̃h�b�g�ʒu�̃|�C���^�[
	@param[in]	pdLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in]	ptStr	�ǉ�������������
	@param[in]	cchSize	������̕������E�k���^�[�~�l�[�^�܂܂�
	@param[out]	*ppstPt	�e�}���ꏊ������o�b�t�@�̃|�C���^���|�C���^
	@return		INT		���������s��
*/
INT DocSquareAdd( PINT pNowDot, PINT pdLine, LPTSTR ptStr, INT cchSize, LPPOINT *ppstPt )
{
	LPTSTR		ptCaret, ptSprt;
	UINT_PTR	cchMozi;
	INT			dCrLf;
	INT			dBaseDot, dBaseLine;

	LPPOINT	pstBuf;

	dCrLf = 0;

	ptCaret = ptStr;
	dBaseLine = *pdLine;

	do
	{
		dBaseDot  = *pNowDot;
		DocLetterPosGetAdjust( &dBaseDot, dBaseLine, 0 );	//	�ꏊ���킹

		ptSprt = StrStr( ptCaret, CH_CRLFW );	//	���s�̂Ƃ���܂�
		if( !(ptSprt) ){	ptSprt = ptStr + cchSize;	}
		//	���[�܂ŉ��s���Ȃ�������A���[�����̈ʒu������
		cchMozi = ptSprt - ptCaret;	//	�����܂ł̕��������߂�
		
		//	�e�s�̒ǉ��ꏊ���o���Ă���
		pstBuf = (LPPOINT)realloc( *ppstPt, ( sizeof(POINT) * (dCrLf+1) ) );

		if( pstBuf ){	*ppstPt = pstBuf;	}
		else{	TRACE( TEXT("fatal mem error") );	return 0;	}
		pstBuf += dCrLf;

		pstBuf->x = dBaseDot;
		pstBuf->y = dBaseLine;
		DocStringAdd( &dBaseDot, &dBaseLine, ptCaret, cchMozi );

		ptCaret = NextLine( ptSprt );	//	���̍s�̐擪�Ɉړ�
		if( *ptCaret  ){	dBaseLine++;	}	//	�s�ʒu���i�߂�

		dCrLf++;

	}while( *ptCaret  );	//	�f�[�^�L����胋�[�v�ŒT��

	*pdLine  = dBaseLine;	//	���[�ʒu�������߂�
	*pNowDot = dBaseDot;

	return dCrLf;
}
//-------------------------------------------------------------------------------------------------

/*!
	���ݕł̖��[�ɉ��s��ǉ�����
	@param[in]	addLine	�ǉ�����s��
	@param[in]	bFirst	�A���h�D�̔�O���߂ẴO���[�v�@�O�����̏���
	@return		�S�̂̍s��
*/
INT DocAdditionalLine( INT addLine, BOOLEAN bFirst )
{
	UINT_PTR	iLines;
	INT			cbSize, cchMozi, i;
	INT			dBaseDot, dBaseLine;
	LPTSTR		ptBuffer = NULL;

	iLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );
	//	���̕ł̍s��

	//	�ǉ�����͍̂ŏI�s�̖��[
	dBaseLine = iLines - 1;

	cchMozi = CH_CRLF_CCH * addLine;	//	���s�̕������{�ʂ邽�[�݂ˁ[��
	cbSize  = (cchMozi + 1) * sizeof(TCHAR);
	ptBuffer = (LPTSTR)malloc( cbSize );

	ZeroMemory( ptBuffer, cbSize );
	for( i = 0; addLine >  i; i++ )
	{
		StringCchCat( ptBuffer, cchMozi + 1, CH_CRLFW );
		ViewRedrawSetLine( dBaseLine + i  );	//	�ĕ`��s���m�肵�Ă���
	}

	dBaseDot = DocLineParamGet( dBaseLine, NULL, NULL );
	SqnAppendString( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_INSERT, ptBuffer, dBaseDot, dBaseLine, bFirst );
	DocStringAdd( &dBaseDot, &dBaseLine, ptBuffer, cchMozi );

	FREE( ptBuffer );

	return iLines;
}
//-------------------------------------------------------------------------------------------------

/*!
	��`�\�t������O�ɁA	��̏󋵂��m�F���āA�K�v�Ȃ琮�`����
	@param[in]	xDot		�ǉ��J�n����h�b�g�ʒu
	@param[in]	yLine		�ǉ��J�n����s�ԍ�
	@param[in]	dNeedLine	�g���s��
	@param[in]	bFirst		�A���h�D�̔�O���߂ẴO���[�v�@�O�����̏���
	@return		INT		��O���Ȃ������@�O��������
*/
INT DocSquareAddPreMod( INT xDot, INT yLine, INT dNeedLine, BOOLEAN bFirst )
{
//	�s���₷�̂ƁA����̈ʒu�܂ŃX�y�[�X�Ŗ��߂�
	INT_PTR	iLines;
	INT		iBaseDot, iBaseLine, iMinus, i;
	UINT	cchBuf;
	LPTSTR	ptBuffer = NULL;


	//	���̕ł̍s��
	iLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );

	//	�S�̍s�����A�ǉ��s��������������A���s���₷
	if( iLines < (dNeedLine + yLine) )
	{
		iMinus = (dNeedLine + yLine) - iLines;	//	�ǉ�����s��

		DocAdditionalLine( iMinus, bFirst );	bFirst = FALSE;

		//	���̕ł̍s����蒼��
		iLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );
	}

	//	�e�s�̃h�b�g�����m�F���āA����Ȃ��Ƃ�����p�f�B���O����
	//�������ɂ͖�薳���E����Ȃ��̂��R�ȉ��Ȃ疳���A���Ƃ͓K���Ƀp�f�B���O�����
	//�ǂ������̐������������瑝�����č���܂ł�����
	for( i = 0; dNeedLine > i; i++ )
	{
		iBaseLine = yLine + i;
		iBaseDot  = DocLineParamGet( iBaseLine, NULL, NULL );
		//	����瑶�݃h�b�g�������ƁA�{�Ȃ瑫��Ȃ�
		iMinus    = xDot - iBaseDot;

		if( gbUniPad  ){	if( 0 >= iMinus )	continue;	}
		else{	if( 3 >= iMinus )	continue;	}	//	�]�邩�R�ȉ��Ȃ�C�ɂ���K�v�͖���

		ptBuffer = DocPaddingSpaceWithPeriod( iMinus, NULL, NULL, NULL, FALSE );
		if( !ptBuffer )	//	�܂����Y��ɔ[�߂�̂����݂āA�_���Ȃ�Y������ōČv�Z
		{	ptBuffer = DocPaddingSpaceWithGap( iMinus, NULL, NULL );	}
		if( !ptBuffer ){	continue;	}
		StringCchLength( ptBuffer, STRSAFE_MAX_CCH, &cchBuf );

		SqnAppendString( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_INSERT, ptBuffer, iBaseDot, iBaseLine, bFirst );
		bFirst = FALSE;
		DocStringAdd( &iBaseDot, &iBaseLine, ptBuffer, cchBuf );

		FREE( ptBuffer );
	}


	return bFirst;
}
//-------------------------------------------------------------------------------------------------

/*!
	������i��`���A���j��}������
	@param[in,out]	pNowDot	���̃L�����b�g�̃h�b�g�ʒu
	@param[in,out]	pdLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in,out]	pdMozi	���̃L�����b�g�̕������ENULL�ł�����
	@param[in]		ptText	�}�����镶����
	@param[in]		dStyle	��`���ǂ����A�s�����ʂ�
	@param[in]		bFirst	�A���h�D�p�E���ꂪ�ŏ��̃A�N�V������
	@return		�O��s�ł��񂾁@��O�����s�ɓn����
*/
INT DocInsertString( PINT pNowDot, PINT pdLine, PINT pdMozi, LPTSTR ptText, UINT dStyle, BOOLEAN bFirst )
{
	INT		dBaseDot, dBaseLine, dNeedLine;
	INT		dCrLf, i, dLastLine;
	UINT	cchSize;
	LPPOINT	pstPoint;

	dBaseDot  = *pNowDot;
	dBaseLine = *pdLine;
	dLastLine = *pdLine;

	StringCchLength( ptText, STRSAFE_MAX_CCH, &cchSize );

	if( dStyle &  D_SQUARE )	//	��`�p
	{
		//	�g���s��
		dNeedLine = DocLineCount( ptText, cchSize );

		bFirst = DocSquareAddPreMod( *pNowDot, *pdLine, dNeedLine, bFirst );
		//	���ŃA���h�D�ǉ��܂ł��B

		pstPoint = NULL;	//	NULL���K�{
		dCrLf = DocSquareAdd( pNowDot, pdLine, ptText, cchSize, &pstPoint );

		//	�\�t�O�̐��`���܂߂ĂPGroup�Ƃ��Ĉ����K�v������
		SqnAppendSquare( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_INSERT, ptText, pstPoint, dCrLf, bFirst );
		bFirst = FALSE;

		FREE( pstPoint );

		dLastLine = *pdLine;
	}
	else
	{
		//	���̒��ŉ��s�Ƃ��ʓ|����
		dCrLf = DocStringAdd( pNowDot, pdLine, ptText, cchSize );

		SqnAppendString( &(gstFile.vcCont.at( gixFocusPage ).stUndoLog), DO_INSERT, ptText, dBaseDot, dBaseLine, bFirst );
		bFirst = FALSE;

		dLastLine = DocPageParamGet( NULL, NULL );
	}

	if( dCrLf )
	{
		for( i = dBaseLine; dLastLine >= i; i++ ){	ViewRedrawSetLine(  i );	}
	}
	else
	{
		ViewRedrawSetLine( *pdLine );
	}

	if( pdMozi ){	*pdMozi = DocLetterPosGetAdjust( pNowDot, *pdLine , 0 );	}

	//	���o�C��Ԃ̂Ƃ��͑��삵�Ȃ��悤�ɂ���
	if( !(D_INVISI & dStyle) )	ViewDrawCaret( *pNowDot, *pdLine, TRUE );


	return dCrLf;
}
//-------------------------------------------------------------------------------------------------

/*!
	�N���b�v�{�[�h�̕������}������E������\��t��
	@param[in,out]	pNowDot	���̃L�����b�g�̃h�b�g�ʒu
	@param[in,out]	pdLine	�Ώۂ̍s�ԍ��E��΂O�C���f�b�N�X��
	@param[in,out]	pdMozi	���̃L�����b�g�̕�����
	@return		�O��s�ł��񂾁@��O�����s�ɓn����
*/
INT DocInputFromClipboard( PINT pNowDot, PINT pdLine, PINT pdMozi )
{
	LPTSTR	ptString = NULL;
	UINT	cchSize, dStyle = 0, i, j;
	INT		dCrLf, dTop, dBtm;
	BOOLEAN	bSelect;
	UINT	dSqSel, iLines;


	//	�N���b�v�{�[�h����f�[�^�𒸂�
	ptString = DocClipboardDataGet( &dStyle );
	if( !(ptString) )
	{
		NotifyBalloonExist( NULL, TEXT("�L���ȃf�[�^���Ȃ��̂ł��B��������"), TEXT("�\�t���Ȃ��̂ł�"), NIIF_INFO );
		return 0;
	}

	StringCchLength( ptString, STRSAFE_MAX_CCH, &cchSize );

	//	�^�u���k��
	for( i = 0; cchSize > i; )
	{
		if( CC_TAB == ptString[i] )
		{
			for( j = i; cchSize > j; j++ )
			{
				ptString[j] = ptString[j+1];
			}
			cchSize--;
			continue;
		}
		i++;
	}

	bSelect = IsSelecting( &dSqSel );	//	�I����Ԃł��邩
	if( bSelect )
	{
		iLines = DocPageParamGet( NULL, NULL );
		DocSelRangeGet( &dTop, &dBtm );
		dCrLf = DocSelectedDelete( pNowDot, pdLine, dSqSel );
		if( dCrLf  )	//	���������s�ȍ~�S����ւ�
		{
			for( i = *pdLine; iLines > i; i++ ){	ViewRedrawSetLine(  i );	}
		}
		else{	ViewRedrawSetLine( *pdLine );	}

	}

	dCrLf = DocInsertString( pNowDot, pdLine, pdMozi, ptString, dStyle, TRUE );

	FREE( ptString );

	DocPageInfoRenew( -1, 1 );

	return dCrLf;
}
//-------------------------------------------------------------------------------------------------

/*!
	�I��͈͂̃f�[�^���N���b�v�{�[�h����
	@param[in]	bStyle	�P���j�R�[�h���V�t�gJIS�ŁA��`���ǂ���
	@return				�R�s�[�����o�C�g���ENULL�^�[�~�l�[�^���܂�
*/
INT DocExClipSelect( UINT bStyle )
{
	INT	cbSize;
	LPVOID	pString = NULL;

	//	SJIS�̏ꍇ�́A���j�R�[�h������&#dddd;�Ŋm�ۂ����

	cbSize = DocSelectTextGetAlloc( bStyle, &pString, NULL );

	TRACE( TEXT("BYTE:%d"), cbSize );

	//	�����I��͈͂Ȃ�������AFocus�s�̓��e���R�s��Ƃ�

	DocClipboardDataSet( pString, cbSize, bStyle );

	FREE( pString );

	return cbSize;
}
//-------------------------------------------------------------------------------------------------

/*!
	�N���b�v�{�[�h�̃f�[�^����������
	@param[in]	pdStyle	��`���ǂ������m�F
	@return		�m�ۂ���������Emalloc���Ă�̂ŁA�����Ă񂾕���free�Y��Ȃ��悤��
*/
LPTSTR DocClipboardDataGet( PUINT pdStyle )
{
	LPTSTR	ptString = NULL, ptClipTxt;
	LPSTR	pcStr, pcClipTp;	//	�ϊ��p�Վ�
	DWORD	cbSize;
	UINT	ixSqrFmt, dEnumFmt;
	INT		ixCount, iC;
	HANDLE	hClipData;

	ixSqrFmt = RegisterClipboardFormat( CLIP_SQUARE );

	//	�N���b�v�{�[�h�̒��g���`�F�L�E�ǂ����ɂ��Ă����j�R�[�h�e�L�X�g�t���O�͂���
	if( IsClipboardFormatAvailable( CF_UNICODETEXT ) )
	{
		if( pdStyle )	//	��`�ł��邩
		{
			if( IsClipboardFormatAvailable( ixSqrFmt ) ){	*pdStyle = D_SQUARE;	}
		}

		OpenClipboard( NULL );	//	�N���b�v�{�[�h���I�[�|������
		//	�J�����������Ƒ��̃A�v���ɖ��f�Ȃ̂ł����߂�悤��

		dEnumFmt = 0;	//	�����l�͂O
		ixCount = CountClipboardFormats(  );
		for( iC = 0; ixCount > iC; iC++ )
		{	//	���Ԃɗ񋓂��āA��Ƀq�b�g�����t�H�[�}�b�g�ň���
			dEnumFmt = EnumClipboardFormats( dEnumFmt );
			if( CF_UNICODETEXT == dEnumFmt || CF_TEXT == dEnumFmt ){	break;	}
		}
		if( 0 == dEnumFmt ){	return NULL;	}
		//	����ȏ�񋓂��������A�������s�Ȃ�O�ɂȂ�

		//	�N���b�v�{�[�h�̃f�[�^���Q�b�c�I
		//	�n���h���̃I�[�i�[�̓N���b�v�{�[�h�Ȃ̂ŁA�����炩��͑��삵�Ȃ��悤��
		//	���g�̕ύX�Ȃǂ����Ă̂ق��ł���
		hClipData = GetClipboardData( dEnumFmt );

		if( CF_UNICODETEXT == dEnumFmt )
		{
			//	�擾�f�[�^�������BTEXT�Ȃ�A�n���h���̓O���[�o���������n���h��
			//	�V���ɃR�s�[���ꂽ��n���h���͖����ɂȂ�̂ŁA���g���R�s�[����
			ptClipTxt = (LPTSTR)GlobalLock( hClipData );
			cbSize    = GlobalSize( (HGLOBAL)hClipData );
			//	�m�ۏo����̂̓o�C�g�T�C�Y�E�e�L�X�g���Ɩ�����NULL�^�[�~�l�[�^�܂�

			if( 0 < cbSize )
			{
				ptString = (LPTSTR)malloc( cbSize );
				StringCchCopy( ptString, (cbSize / 2), ptClipTxt );
			}
		}
		else	//	�񃆃j�R�[�h���D�悳��Ă���ꍇ
		{
			pcClipTp = (LPSTR)GlobalLock( hClipData );
			cbSize   = GlobalSize( (HGLOBAL)hClipData );

			if( 0 < cbSize )
			{
				pcStr = (LPSTR)malloc( cbSize );
				StringCchCopyA( pcStr, cbSize, pcClipTp );

				ptString = SjisDecodeAlloc( pcStr );	//	SJIS�̓��e�����j�R�[�h�ɂ���
				free( pcStr );
			}
		}


		//	�g���I���������Ă���
		GlobalUnlock( hClipData );
		CloseClipboard(  );
	}

	return ptString;
}
//-------------------------------------------------------------------------------------------------

/*!
	�N���b�v�{�[�h�ɕ�����\��t��
	@param[in]	pDatum	�\��t������������E���j��SJIS
	@param[in]	cbSize	������́ANULL���܂񂾃o�C�g��
	@param[in]	dStyle	��`���Ƃ����������w��
	@return	HRESULT	�I����ԃR�[�h
*/
HRESULT DocClipboardDataSet( LPVOID pDatum, INT cbSize, UINT dStyle )
{
	HGLOBAL	hGlobal;
	HANDLE	hClip;
	LPVOID	pBuffer;
	HRESULT	hRslt;
	UINT	ixFormat, ixSqrFmt;

	//	�I���W�i���t�H�[�}�b�g�����`���Ă���
	ixFormat = RegisterClipboardFormat( CLIP_FORMAT );
	ixSqrFmt = RegisterClipboardFormat( CLIP_SQUARE );

	//	�N���b�v����f�[�^�͋��L�������ɓ����
	hGlobal = GlobalAlloc( GHND, cbSize );
	pBuffer = GlobalLock( hGlobal );
	CopyMemory( pBuffer, pDatum, cbSize );
	GlobalUnlock( hGlobal );

	//	�N���b�v�{�[�h�I�[�|��
	OpenClipboard( NULL );

	//	���g���������Ⴄ
	EmptyClipboard(  );

	//	���L�������Ƀu�b���񂾃f�[�^���N���b�y����
	if( dStyle & D_UNI )	hClip = SetClipboardData( CF_UNICODETEXT, hGlobal );
	else					hClip = SetClipboardData( CF_TEXT, hGlobal );

	if( hClip )
	{
		SetClipboardData( ixFormat, hGlobal );
		//	�N���b�|����肭��������A�I���W�i�����ł��L�^���Ă���

		if( dStyle & D_SQUARE  )	//	��`�I����������
		{
			SetClipboardData( ixSqrFmt, hGlobal );
		}

		hRslt = S_OK;
	}
	else
	{
		//	�o�^���s�̏ꍇ�́A�����ŋ��L��������j�󂹂Ȃ�����
		GlobalFree( hGlobal );
		hRslt = E_OUTOFMEMORY;
	}

	//	�N���b�v�{�[�h����E�����瑦CHAIN����������E�������Ŕ��������Ă�H
	CloseClipboard(  );

	TRACE( TEXT("COPY DONE") );

	return hRslt;
}
//-------------------------------------------------------------------------------------------------

/*!
	�N���b�v�{�[�h�ɂP�������������
	@param[in]	ch	���ꂽ������
	@return	HRESULT	�I����ԃR�[�h
*/
HRESULT DocClipLetter( TCHAR ch )
{
	TCHAR	atBuff[3];

	ZeroMemory( atBuff, sizeof(atBuff) );
	atBuff[0] = ch;

	DocClipboardDataSet( atBuff, 4, D_UNI );

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�őS�̂��R�s�[����
	@param[in]	bStyle	���j�R�[�h���V�t�gJIS
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT DocPageAllCopy( UINT bStyle )
{
	INT	cbSize;
	LPVOID	pString = NULL;

	//	SJIS�̏ꍇ�́A���j�R�[�h������&#dddd;�Ŋm�ۂ����

	cbSize = DocPageTextAllGetAlloc( bStyle, &pString );

	TRACE( TEXT("BYTE:%d"), cbSize );

	DocClipboardDataSet( pString, cbSize, bStyle );

	FREE( pString );

	return S_OK;
}
//-------------------------------------------------------------------------------------------------


/*!
	�e�s�̖��[����W�O�O���炢�܂ł��A�w�肵�������Ŗ��߂�B
	@param[in]	ptFill	���ߕ�����
*/
HRESULT DocScreenFill( LPTSTR ptFill )
{
	UINT_PTR	dLines, dRiDot, cchSize;
	BOOLEAN		bSel = TRUE, bFirst;
	INT			iTop, iBottom, i, iUnt, j, remain;
	INT			nDot, sDot, mDot;
	LPTSTR		ptBuffer;
	wstring		wsBuffer;

	//	���ݍs���ƁA�E�h�b�g���E���[���ʒu���g��
	dLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );
	dRiDot = gdRightRuler;

	//	�I��͈͂���Ȃ炻�����D��B�Ȃ��Ȃ��ʑS��
	iTop    = gstFile.vcCont.at( gixFocusPage ).dSelLineTop;
	iBottom = gstFile.vcCont.at( gixFocusPage ).dSelLineBottom;
	if( 0 > iTop || 0 > iBottom ){	iTop = 0;	iBottom = dLines - 1;	bSel = FALSE;	}

	ViewSelPageAll( -1 );	//	�I��͈͖����Ȃ�

	//	���ߕ�����̕�
	mDot = ViewStringWidthGet( ptFill );

	bFirst = TRUE;
	//	�e�s���ɒǉ����銴����
	for( i = iTop; iBottom >= i; i++ )
	{
		nDot = DocLineParamGet( i , NULL, NULL );	//	�Ăяo���Β��Ŗʓ|�݂Ă����
		sDot = dRiDot - nDot;	//	�c��h�b�g
		if( 0 >= sDot ){	continue;	}	//	�E�[�����Ă�Ȃ�Ȃɂ�����ł���

		iUnt = (sDot / mDot) + 1;	//	���߂镪�E�͂ݏo���E�K���ŗǂ�

		//	����镶����쐬
		wsBuffer.clear( );
		for( j = 0; iUnt > j; j++ ){	wsBuffer += wstring( ptFill );	}

		cchSize = wsBuffer.size( ) + 1;
		ptBuffer = (LPTSTR)malloc( cchSize * sizeof(TCHAR) );
		StringCchCopy( ptBuffer, cchSize, wsBuffer.c_str( ) );
		//	���[�Ƀu�`����
		DocInsertString( &nDot, &i, NULL, ptBuffer, 0, bFirst );	bFirst = FALSE;
		FREE(ptBuffer);

		DocBadSpaceCheck( i );	//	�����ŋ󔒃`�F�L�E���܂�Ӗ��͂Ȃ����F�����͕K�v
	}

	if( !(bSel) )	//	��I����Ԃ�
	{
		remain = 40 - dLines;	//	�Ƃ肠�����S�O�s�Ƃ���
		if( 0 < remain )	//	����Ȃ��Ȃ�
		{
			DocAdditionalLine( remain, bFirst );	//	�Ƃ肠�������s����
			dLines = gstFile.vcCont.at( gixFocusPage ).vcPage.size( );

			iUnt  = (dRiDot / mDot) + 1;	//	���߂镪�E�͂ݏo���E�K���ŗǂ�

			//	����镶����쐬
			wsBuffer.clear( );
			for( j = 0; iUnt > j; j++ ){	wsBuffer += wstring( ptFill );	}
			cchSize = wsBuffer.size( ) + 1;
			ptBuffer = (LPTSTR)malloc( cchSize * sizeof(TCHAR) );
			StringCchCopy( ptBuffer, cchSize, wsBuffer.c_str( ) );

			iTop    = iBottom + 1;
			iBottom = dLines - 1;

			for( i = iTop; iBottom >= i; i++ )
			{
				//	���[�Ƀu�`����
				nDot = DocLineParamGet( i , NULL, NULL );	//	�����O�̃n�Y
				DocInsertString( &nDot, &i, NULL, ptBuffer, 0, bFirst );	bFirst = FALSE;

				DocBadSpaceCheck( i );	//	�����ŋ󔒃`�F�L�E���܂�Ӗ��͂Ȃ����F�����͕K�v
			}

			FREE(ptBuffer);
		}
	}

	DocPageInfoRenew( -1, 1 );

	return S_OK;
}
//-------------------------------------------------------------------------------------------------


/*!
	�Ŕԍ��}���ݒ�_�C�����O�̃��b�Z�[�W�n���h���ł��B
	@param[in]	hDlg		�_�C�����O�n���h��
	@param[in]	message		�E�C���h�E���b�Z�[�W�̎��ʔԍ�
	@param[in]	wParam		�ǉ��̏��P
	@param[in]	lParam		�ǉ��̏��Q
	@retval 0	���b�Z�[�W�͏������Ă��Ȃ�
	@retval no0	�Ȃ񂩏������ꂽ
*/
INT_PTR CALLBACK PageNumDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	static LPPAGENUMINFO	pstInfo;
	INT		id;
	HWND	hWndCtl;
	UINT	codeNotify;

	switch( message )
	{
		case WM_INITDIALOG:
			pstInfo = (LPPAGENUMINFO)lParam;
			SetDlgItemInt(  hDlg, IDE_PN_START, pstInfo->dStartNum, FALSE );
			SetDlgItemText( hDlg, IDE_PN_STYLE, pstInfo->atStyle );
		//	CheckDlgButton( hDlg, IDCB_PN_UNDER, pstInfo->bInUnder ? BST_CHECKED : BST_UNCHECKED );
			//	�㏑���͒ʏ�n�e�e�ł�낵
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			id = LOWORD(wParam);
			hWndCtl = (HWND)lParam;
			codeNotify = HIWORD(wParam);

			if( IDOK == id )
			{
				pstInfo->dStartNum = GetDlgItemInt( hDlg, IDE_PN_START, NULL, FALSE );
				GetDlgItemText( hDlg, IDE_PN_STYLE, pstInfo->atStyle, MIN_STRING );
				pstInfo->bInUnder  = IsDlgButtonChecked( hDlg, IDCB_PN_UNDER );
				pstInfo->bOverride = IsDlgButtonChecked( hDlg, IDCB_PN_OVERRIDE );
				EndDialog(hDlg, IDOK );
				return (INT_PTR)TRUE;
			}
			if( IDCANCEL == id )
			{
				EndDialog(hDlg, IDCANCEL );
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}
//-------------------------------------------------------------------------------------------------

/*!
	�Ŕԍ��}���̃A��
	@param[in]	hInst	�A�v���̎���
	@param[in]	hWnd	�e�E�C���h�E�n���h���ENULL�Ŕj�󏈗�
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT DocPageNumInsert( HINSTANCE hInst, HWND hWnd )
{
	INT			dNowPageBuffer;
	INT			iLine, iDot;
	INT_PTR		iRslt, maxPage, iNow;
	UINT		ixNumber;
	BOOLEAN		bFirst = TRUE;
	TCHAR		atText[MAX_PATH];
	PAGENUMINFO	stInfo;

	//	���̕ł�Ҕ�
	dNowPageBuffer = gixFocusPage;

	maxPage = DocPageCount(  );

	ZeroMemory( &stInfo, sizeof(PAGENUMINFO) );
	stInfo.dStartNum = 1;

	StringCchCopy( stInfo.atStyle, MAX_PATH, TEXT("%u") );	//	�����l
	InitParamString( INIT_LOAD, VS_PAGE_FORMAT, stInfo.atStyle );

	iRslt = DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_PAGENUMBER_DLG), hWnd, PageNumDlgProc, (LPARAM)(&stInfo) );
	if( IDOK == iRslt )
	{
		ixNumber = stInfo.dStartNum;

		InitParamString( INIT_SAVE, VS_PAGE_FORMAT, stInfo.atStyle );

		for( iNow = 0; maxPage > iNow; iNow++, ixNumber++ )
		{
			StringCchPrintf( atText, MAX_PATH, stInfo.atStyle, ixNumber );

			gixFocusPage = iNow;	//	��������

			if( stInfo.bInUnder )
			{
				if( stInfo.bOverride )
				{
					iLine = DocPageParamGet( NULL, NULL );
					iLine--;	if( 0 > iLine ){	iLine = 0;	}
					bFirst = DocLineErase( iLine, bFirst );
				}
				else
				{
					iLine = DocAdditionalLine( 1 , bFirst );	bFirst =  FALSE;
				}
			}
			else
			{
				iDot = 0;	iLine = 0;
				if( stInfo.bOverride )
				{
					bFirst = DocLineErase( 0, bFirst );
				}
				else
				{
					DocInsertString( &iDot, &iLine, NULL, CH_CRLFW, 0, bFirst );	bFirst = FALSE;
				}
				iLine = 0;
			}
			iDot = 0;

			DocInsertString( &iDot, &iLine, NULL, atText, 0, bFirst );	bFirst = FALSE;
		}

		//	�Ō��ɖ߂�
		gixFocusPage = dNowPageBuffer;

		ViewRedrawSetLine( -1 );
	}

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

