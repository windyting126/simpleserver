// encryption_algorithm.h: interface for the encryption_algorithm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCRYPTION_ALGORITHM_H__D7B1F5C8_E8AC_46A6_9483_8978C002BEB6__INCLUDED_)
#define AFX_ENCRYPTION_ALGORITHM_H__D7B1F5C8_E8AC_46A6_9483_8978C002BEB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class encryption_algorithm  
{
public:
	encryption_algorithm();
	virtual ~encryption_algorithm();

	/**
	@purpose    : ������Կ
	@param	data: ���ڴ�����Կ������
	@param	len : ���ڴ�����Կ�����ݵĳ���
	@param  key	: ������Կ
	@param  key_len : ������Կ����
	@return		: �Ƿ񴴽��ɹ�
	*/
	virtual bool create_key( const unsigned char *data, unsigned len, void *key, unsigned &key_len ) = 0;
	
	/**
	@purpose			: ������Ҫ���͵����� 
	@param	io_buffer	: ����
	@param  len			: ���ݵĳ���
	@return				: ���ؽ��ֵ
	*/
	virtual bool encrypt( unsigned char *io_buffer, unsigned len ) = 0;

	/**
	@purpose			: ���յ������ݽ���
	@param	io_buffer	: ����
	@param  len			: ���ݵĳ���
	@return				: ���ؽ��ֵ
	*/
	virtual bool decrypt( unsigned char *io_buffer, unsigned len ) =0;
};

#endif // !defined(AFX_ENCRYPTION_ALGORITHM_H__D7B1F5C8_E8AC_46A6_9483_8978C002BEB6__INCLUDED_)
