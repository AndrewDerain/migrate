#pragma once
#include "basic_type.h"

_MANGO_PACK_PUSH
_MANGO_DETAIL_NAMESPACE_BEGIN
namespace interprocess
{
	namespace eipc
	{
		struct content
		{
			friend class _basic_lock_free_cyclic_queue;

		private:
			tag_t m_tag;      //< 用户自定义数据标示符号
			size_t m_size;    //< 数据大小
			char* m_beg_addr; //< 数据段的起始地址
			offset_t m_data;  //< 数据的起始地址

		public:
			/// 用户自定义数据标示符号
			tag_t tag() { return m_tag; }
			/// 数据大小
			size_t size() { return m_size; }
			/// 数据的起始地址
			void* data() { return m_beg_addr + m_data; }
		};
	}
}
_MANGO_DETAIL_NAMESPACE_END
_MANGO_PACK_POP


_MANGO_PACK_PUSH
_MANGO_NAMESPACE_BEGIN
namespace interprocess
{
	namespace eipc
	{
		typedef int64_t tag_t;
		typedef int64_t size_t;
		typedef int64_t offset_t;
		
		typedef _mangodetail::interprocess::eipc::content* content_ptr;
	}
}
_MANGO_NAMESPACE_END
_MANGO_PACK_POP
