// mhxy_udp.h: interface for the mhxy_udp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHXY_UDP_H__5FEDF880_201D_4217_89B2_7D430E8942A2__INCLUDED_)
#define AFX_MHXY_UDP_H__5FEDF880_201D_4217_89B2_7D430E8942A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MHXY_ENCRYPT_EXTRA_LEN 20

class mhxy_udp  
{
public:
	mhxy_udp();
	virtual ~mhxy_udp();

	// ��һ�׽ӿ�

	/**
	@purpose			: �������� 
	@param	type		: ���ܷ���
	@param	in_buffer	: ����ܵ�����
	@param	in_len		: ����ܵ����ݳ���
	@param	out_buffer	: ���ܺ������
	@param	out_len		: ���ܺ�����ݳ���
	@return				: ���ؽ��ֵ
	*/
	static void encrypt(int type, unsigned char *in_buffer, unsigned in_len, unsigned char *out_buffer, unsigned &out_len);

	/**
	@purpose			: �������� 
	@param	in_buffer	: ����ܵ�����
	@param	in_len		: ����ܵ����ݳ���
	@param	out_buffer	: ���ܺ������
	@param	out_len		: ���ܺ�����ݳ���
	@return				: ���ؼ��ܷ���
	*/
	static int decrypt(unsigned char *in_buffer, unsigned in_len, unsigned char *out_buffer, unsigned &out_len);

	// �ڶ��׽ӿ�
	
	/**
	@purpose			: �������� 
	@param	type		: ���ܷ���
	@param	io_buffer	: ����ǰ�������
	@param	io_len		: ����ǰ������ݳ���
	@return				: ���ؽ��ֵ
	*/
	static void encrypt(int type, unsigned char *io_buffer, unsigned &io_len);

	/**
	@purpose			: �������� 
	@param	io_buffer	: ����ǰ�������
	@param	io_len		: ����ǰ������ݳ���
	@return				: ���ؼ��ܷ���
	*/
	static int decrypt(unsigned char *io_buffer, unsigned &io_len);
};

#endif // !defined(AFX_MHXY_UDP_H__5FEDF880_201D_4217_89B2_7D430E8942A2__INCLUDED_)
