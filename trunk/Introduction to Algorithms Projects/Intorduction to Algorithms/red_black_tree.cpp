//////////////////////////////////////////////////////////////////////////  
///    COPYRIGHT NOTICE  
///    Copyright (c) 2009
///    All rights reserved.  
///  
/// @file		red_black_tree.cpp
/// @brief		red_black_tree.cpp�ļ������ 
///  			
///  			red_black_tree.cpp����ϸ����
/// 
/// @author		̷����	chuanqi.tan(at)gmail.com 
/// @date		2011/05/30
/// @version 	1.0
///  
///  
///    �޶�˵��������汾  
//////////////////////////////////////////////////////////////////////////  

#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <iomanip>
#include <limits>
#include <fstream>
using namespace std;

namespace chapter13
{
	/// �����
	template<typename TKey, typename TValue>
	class RBTree
	{
	public:
		/// ������н����ɫ��ö��
		enum RBTreeNodeColor
		{
			BLACK,			///< ��ɫ
			RED				///< ��ɫ			
		};

		/// ������еĽ��
		struct RBTreeNode
		{
			TKey				Key;
			TValue				Value;
			RBTreeNodeColor		Color;
			RBTreeNode			*Parent;
			RBTreeNode			*Left;
			RBTreeNode			*Right;

			/// ����Ƿ�Ϊ���ڱ���㣨��nil��
			/// @return			�ý���Ƿ�Ϊ��Ч��㣬����Ϊnil���
			/// @retval	true	��nil���
			/// @retval false	nil���
			inline bool IsValid() const
			{
				return (this != s_nil);
			}
		};
	
		RBTree()
		{
			if (!s_nil)
			{
				//Ҷ�ӽ����һ������ĺڽ��
				s_nil = new RBTreeNode();
				s_nil->Color = BLACK;
			}

			_root = s_nil;
		}

		~RBTree()
		{
			_RecursiveReleaseNode(_root);
		}

		bool Insert(TKey key, TValue value)
		{
			if (Search(key)->IsValid())
			{//key�ظ�������ʧ��
				return false;
			}
			else
			{
				//�����ӵĽ��Ϊ���㣬��Left=Right=s_nil
				RBTreeNode *new_node = new RBTreeNode();
				new_node->Key = key;
				new_node->Value = value;
				new_node->Color = RED;
				new_node->Left = new_node->Right = s_nil;

				_InsertAsNormalBSTree(new_node);
				_InsertFixup(new_node);
				return true;
			}
		}

		bool Delete(TKey key)
		{
			if (Search(key)->IsValid())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		RBTreeNode * Search(TValue const &value)
		{
			RBTreeNode *node = _root;
			while (node != s_nil && node->Value != value)
			{
				node = (value < node->Value ? node->Left : node->Right); 
			}
			return node;
		}

		bool Empty()
		{
			return !(_root->IsValid());
		}

		/// ��ʾ��ǰ�����������״̬
		/// 
		/// ʹ��dot������ǰ�����
		void Display() const
		{
			ofstream dot_file("c:\\red_back_tree.dot");

			dot_file << "digraph graphname" << (rand() % 1000) << "{" << endl
				<< "    node [shape = record,height = .1];" << endl;
			_Display(dot_file, _root);
			dot_file << "}" << endl;

			dot_file.close();
			system("dot -Tpng c:\\red_back_tree.dot -o c:\\red_back_tree.png");
			system("c:\\red_back_tree.png");
		}

	private:
		void _RecursiveReleaseNode(RBTreeNode *node)
		{
			if (node->IsValid())
			{			
				_RecursiveReleaseNode(node->Left);
				_RecursiveReleaseNode(node->Right);
				delete node;
			}
		}

		void _Display(ofstream &dot_file, RBTreeNode *node) const
		{
			if (node->IsValid())
			{
				dot_file << "    node" << node->Value << "[label = \"<f0>|<f1>" << node->Value << "|<f2>\", color = " << (node->Color == RED ? "red" : "black") << "];" << endl;

				if (node->Left->IsValid())
				{
					dot_file << "    \"node" << node->Value << "\":f0 -> \"node" << node->Left->Value << "\":f1;" << endl;
					_Display(dot_file, node->Left);
				}

				if (node->Right->IsValid())
				{
					dot_file << "    \"node" << node->Value << "\":f2 -> \"node" << node->Right->Value << "\":f1;" << endl;
					_Display(dot_file, node->Right);
				}
			}
		}

		/// ��һ�����򵥵ؼ�������
		/// 
		/// �Ӹú����Ϊ��ͨ�Ķ���������򵥵Ľ��в����������Ҫ�ڴ�֮���������������������
		/// @notes		һ��Ҫ��֤node->Keyһ����һ���µ�ֵ�����������ѭ���������ﲻ���
		void _InsertAsNormalBSTree( RBTreeNode *node ) 
		{
			if (!_root->IsValid())
			{//������ǵ�1���ڵ�
				_root = node;
				_root->Left = _root->Right = _root->Parent = s_nil;
				_root->Color = BLACK;
				return;
			}

			//�ǵ�1���ڵ�
			RBTreeNode *current_node = _root;

			while (true)
			{
				RBTreeNode *&next_node_pointer = (node->Key > current_node->Key ? current_node->Right : current_node->Left);
				if (next_node_pointer->IsValid())
				{
					current_node = next_node_pointer;
				}
				else
				{//���������Ĳ������
					node->Parent = current_node;
					next_node_pointer = node;
					break;
				}
			}
		}

		/// �Բ������������
		/// 
		/// ���ڶԺ�����Ĳ�������ƻ��˺���������ʣ�������Ҫ������������
		/// @notes		node�Ľ������Ҫ�����Ľ�㣬�������ƻ��˺�����ʣ���һ���Ǻ���
		void _InsertFixup( RBTreeNode * node ) 
		{
			while (node->Parent->Color == RED)
			{
				//��ʶnode�ĸ�����Ƿ�Ϊnode���游��������
				bool parent_is_left_child_flag = (node->Parent == node->Parent->Parent->Left);		
				//������
				RBTreeNode *uncle = parent_is_left_child_flag ? node->Parent->Parent->Right : node->Parent->Parent->Left;
				if (uncle->Color == RED)
				{//case1
					node->Parent->Color = BLACK;
					uncle->Color = BLACK;
					node->Parent->Parent->Color = RED;
					node = node->Parent->Parent;
				}
				else 
				{
					if (node == (parent_is_left_child_flag ? node->Parent->Right : node->Parent->Left))
					{//case2
						node = node->Parent;
						parent_is_left_child_flag ? _LeftRotate(node) : _RightRotate(node);
					}

					//case3
					node->Parent->Color = BLACK;
					node->Parent->Parent->Color = RED;
					parent_is_left_child_flag ? _RightRotate(node->Parent->Parent) : _LeftRotate(node->Parent->Parent);
				}				
			}

			//��������2���ƻ�ֻ��Ҫ��򵥵�һ�仰
			_root->Color = BLACK;
		}

		/// ����
		void _LeftRotate( RBTreeNode * node ) 
		{
			if (!(node->IsValid() && node->Right->IsValid()))
			{//��������Ҫ��Է��ڱ����в���������Ҫ���Һ���Ҳ�����ڱ�
				throw std::exception("��������Ҫ��Է��ڱ����в���������Ҫ���Һ���Ҳ�����ڱ�");
			}
			else
			{
				//node���Һ���
				RBTreeNode *right_son = node->Right;

				node->Right = right_son->Left;
				if (right_son->Left->IsValid())
				{
					right_son->Left->Parent = node;
				}
				right_son->Parent = node->Parent;
				if (!(node->Parent->IsValid()))
				{
					_root = right_son;
				}
				else
				{
					if (node == node->Parent->Left)
					{
						node->Parent->Left = right_son;
					}
					else
					{
						node->Parent->Right = right_son;
					}
				}
				right_son->Left = node;
				node->Parent = right_son;
			}
		}

		/// ����
		void _RightRotate( RBTreeNode * node ) 
		{
			if (!(node->IsValid() && node->Left->IsValid()))
			{//��������Ҫ��Է��ڱ����в���������Ҫ������Ҳ�����ڱ�
				throw std::exception("��������Ҫ��Է��ڱ����в���������Ҫ������Ҳ�����ڱ�");
			}
			else
			{
				//node������
				RBTreeNode *left_son = node->Left;

				node->Left = left_son->Right;
				if (left_son->Right->IsValid())
				{
					left_son->Right->Parent = node;
				}
				left_son->Parent = node->Parent;
				if (!(node->Parent->IsValid()))
				{
					_root = left_son;
				}
				else
				{
					if (node == node->Parent->Left)
					{
						node->Parent->Left = left_son;
					}
					else
					{
						node->Parent->Right = left_son;
					}
				}
				left_son->Right = node;
				node->Parent = left_son;
			}
		}





		RBTreeNode *_root;				///< �����

		static RBTreeNode *s_nil;		///< �������Ҷ�ӽ�㣨�ڱ���
	};


	template<typename TKey, typename TValue>
	typename RBTree<TKey, TValue>::RBTreeNode * RBTree<TKey, TValue>::s_nil = NULL;

	int test()
	{
		int init[] = {11, 2, 14, 1, 7, 15, 5, 8};
		RBTree<int, int> bst;
		for (int i = 0; i < sizeof(init) / sizeof(init[0]); ++i)
		{
			bst.Insert(init[i], init[i]);
		}
		bst.Insert(4, 4);
		bst.Display();

		//�����ֵ����һ�ö��������
		for (int i = 0; i < 100; ++i)
		{
			int v = rand() % 100;
			bst.Insert(v, v);						
		}
		//bst.Insert(99, 99);
		bst.Display();

		return 0;
	}
}