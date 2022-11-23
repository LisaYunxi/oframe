#pragma once
 
#include <map>
 
#if defined  _WIN32                                                         //Windows
#include <Windows.h>
#define sfmap_CLock_Mutex_t                 HANDLE
#define sfmap_CLock_Mutex_Init(_mutex)      (_mutex = CreateSemaphore(NULL,1,1,NULL))
#define sfmap_CLock_Mutex_Lock(_mutex)      (WaitForSingleObject(_mutex, INFINITE))
#define sfmap_CLock_Mutex_UnLock(_mutex)    (ReleaseSemaphore(_mutex,1,NULL))
#define sfmap_CLock_Mutex_Destroy(_mutex)   (CloseHandle(_mutex))
#define sfmap_Declar_Typename               typename
#define sfmap_Type_Typename
 
#else
#include <pthread.h>
#define sfmap_CLock_Mutex_t                 pthread_mutex_t
#define sfmap_CLock_Mutex_Init(_mutex)      (pthread_mutex_init(&_mutex, NULL))
#define sfmap_CLock_Mutex_Lock(_mutex)      (pthread_mutex_lock(&_mutex))
#define sfmap_CLock_Mutex_UnLock(_mutex)    (pthread_mutex_unlock(&_mutex))
#define sfmap_CLock_Mutex_Destroy(_mutex)   (pthread_mutex_destroy(&_mutex))
#define sfmap_Declar_Typename 
#define sfmap_Type_Typename                 typename
#endif
 
//************************************  
// ��������: PthreadSelf
// ����˵���� ��ȡ�߳�ID��������
// �� �� �ߣ�smallErLang  
// �������ڣ�2016/04/22
// �� �� ֵ: unsigned int �����ص�ǰ�̵߳�ID
//************************************
inline unsigned int PthreadSelf()
{
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}
 
namespace sfmap
{
    template<class K, class V>
    class map;
 
    template<class K, class V>
    class myiterator;
 
    //lock
    class CLock  
    {  
    public: 
        CLock(){sfmap_CLock_Mutex_Init(_mutex);}
        ~CLock(){sfmap_CLock_Mutex_Destroy(_mutex);}
 
        void Lock() {sfmap_CLock_Mutex_Lock(_mutex);}
        void UnLock() {sfmap_CLock_Mutex_UnLock(_mutex);}
 
 
    private:
        sfmap_CLock_Mutex_t _mutex;
    };
 
    //threadlockstats
    /************************************************************************/
    /* 
        1.CThreadLockStats�Ǳ����˵�ǰ�����̵߳���_mutex_usr.Lock()��_mutex_usr.UnLock()
        �Ĳ�ֵ,��¼ͬһ�߳���_mutex_usr��ʹ�������
        2.���п��ú���ΪTryLock(),TryUnLock()�����ͬһ�߳���_mutex_usr�Ѿ�Lock����TryLock()
        ��������ֻ�ǽ���ʶ���������ͬһ�߳���_mutex_usr��Lock����Ϊ1ͬʱ��Ҫ�ͷ�����ʱ��
        ����TryUnLock���ͷţ���Ϊ1�򽫱�ʶ�Լ���
    */
    /************************************************************************/
    template<class K, class V>
    class CThreadLockStats
    {
    private:
        CThreadLockStats();
        ~CThreadLockStats();
 
        //************************************  
        // ��������: TryLock
        // ����˵���� ��ֹ����
        // �� �� �ߣ�smallErLang  
        // �������ڣ�2016/04/22
        // �� �� ֵ: void  
        //************************************
        void TryLock();
 
        //************************************  
        // ��������: TryUnLock
        // ����˵���� ��TryLock��Ӧ���ͷ���
        // �� �� �ߣ�smallErLang  
        // �������ڣ�2016/04/22
        // �� �� ֵ: void  
        //************************************
        void TryUnLock();
 
    private:
        CLock _mutex_usr;
 
        CLock _mutex_stats;
 
        std::map<unsigned int, int>* _thread_lock_stats;
 
        friend class sfmap::map<K, V>;
 
        friend class sfmap::myiterator<K, V>;
    };
 
    template<class K, class V>
    void sfmap::CThreadLockStats<K, V>::TryUnLock()
    {
        bool _isneedusrunlock = false;
        unsigned int _thread_id = PthreadSelf();
 
        _mutex_stats.Lock();
 
        if(_thread_lock_stats){
            sfmap_Type_Typename std::map<unsigned int, int>::iterator _finditr = _thread_lock_stats->find(_thread_id);
 
            if(_finditr != _thread_lock_stats->end()){
 
                if(_finditr->second == 1){
 
                    _isneedusrunlock = true;
                }
 
                _finditr->second -= 1;
 
            }else{
 
                int _new_lock_counts = 0;
 
                _thread_lock_stats->insert(sfmap_Type_Typename std::map<unsigned int, int>::value_type(_thread_id, _new_lock_counts));
            }
        }
 
        _mutex_stats.UnLock();
 
        if(_isneedusrunlock){
 
            _mutex_usr.UnLock();
        }
    }
 
    template<class K, class V>
    void sfmap::CThreadLockStats<K, V>::TryLock()
    {
        bool _isneedusrlock = false;
        unsigned int _thread_id = PthreadSelf();
 
        _mutex_stats.Lock();
 
        if(_thread_lock_stats){
 
            sfmap_Type_Typename std::map<unsigned int, int>::iterator _finditr = _thread_lock_stats->find(_thread_id);
 
            if(_finditr != _thread_lock_stats->end()){
 
                if(_finditr->second <= 0){
 
                    _finditr->second = 0;
 
                    _isneedusrlock = true;
                }
 
                _finditr->second += 1;
            }else{
 
                int _new_lock_counts = 1;
 
                _thread_lock_stats->insert(sfmap_Type_Typename std::map<unsigned int, int>::value_type(_thread_id, _new_lock_counts));
 
                _isneedusrlock = true;
            }
        }
 
        _mutex_stats.UnLock();
 
        //��_mutex_usr.Lock������_mutex_stats�ͷź󣬷�ֹ����
        //�����зǳɶԳ��ֵ��������������ڶ��߳�Ƕ�׵���ʱһ�����������
        if(_isneedusrlock){
 
            _mutex_usr.Lock();
        }
    }
 
    template<class K, class V>
    sfmap::CThreadLockStats<K, V>::~CThreadLockStats()
    {
        _mutex_stats.Lock();
 
        delete _thread_lock_stats;
 
        _thread_lock_stats = NULL;
 
        _mutex_stats.UnLock();
    }
 
    template<class K, class V>
    sfmap::CThreadLockStats<K, V>::CThreadLockStats()
    {
        _thread_lock_stats = new std::map<unsigned int, int>;
    }
 
 
    //sfmap
    /************************************************************************/
    /* 
        ����iteratorĬ�Ϲ��캯��û��TryLock�������ڷ��ظ�������ʱ����ʱ��ҪTryLockһ�Σ�
    ��ʱ��������ʱTryUnLockһ�Σ������ص�ֵ���п���������߿�����ֵ��TryLockһ�Σ�ʹ��
    �������TryUnLockһ�Ρ��ﵽ�ɶ������ͷ�����Ŀ�ġ�
    */
    /************************************************************************/
    template<class K, class V>
    class map: public std::map<K, V>
    {
    public:
        map();
        map(const map& val_);
        ~map();
 
        map& operator=(const map& val_);
#if defined  _WIN32   
		typedef typename std::map<K, V>::iterator iterator;
#else
		typedef class std::map<K, V>::iterator iterator;
#endif
        void insert_s(const K& key_, const V& val_);
        void erase_s(const K& key_);
        void erase_s(iterator& itr_);
        iterator find_s(const K& key_);
		
        iterator begin_s();
        iterator end_s();
 
        unsigned int size_s();
 
    private:
        CThreadLockStats<K, V> _mutex_stats;
    };
 
    template<class K, class V>
    unsigned int sfmap::map<K, V>::size_s()
    {
        unsigned int _size = 0;
 
        _mutex_stats.TryLock();
 
        _size = this->size();
 
        _mutex_stats.TryUnLock();
 
        return _size;
    }
 
    template<class K, class V>
    typename sfmap::map<K, V>::iterator sfmap::map<K, V>::end_s()
    {
        sfmap::map<K, V>::iterator _ret;
 
        _mutex_stats.TryLock();
 
        _ret = this->end();
		_mutex_stats.TryUnLock();
        return _ret;
    }
 
    template<class K, class V>
    typename sfmap::map<K, V>::iterator sfmap::map<K, V>::begin_s()
    {
        sfmap::map<K, V>::iterator _ret;
 
        _mutex_stats.TryLock();
 
        _ret = this->begin();
		_mutex_stats.TryUnLock();
        return _ret;
    }
 
    template<class K, class V>
    typename sfmap::map<K, V>::iterator sfmap::map<K, V>::find_s( const K& key_ )
    {
        sfmap::map<K, V>::iterator _ret;
 
        _mutex_stats.TryLock();
 
        _ret = this->find(key_);

		_mutex_stats.TryUnLock();
        return _ret;
    }
 
    template<class K, class V>
    void sfmap::map<K, V>::erase_s( typename sfmap::map<K, V>::iterator& itr_ )
    {
        _mutex_stats.TryLock();
 
        this->erase(itr_);
 
        _mutex_stats.TryUnLock();
    }
 
    template<class K, class V>
    void sfmap::map<K, V>::erase_s( const K& key_ )
    {
        _mutex_stats.TryLock();
 
        this->erase(key_);
 
        _mutex_stats.TryUnLock();
 
        return ;
    }
 
    template<class K, class V>
    void sfmap::map<K, V>::insert_s( const K& key_, const V& val_ )
    {
        _mutex_stats.TryLock();
 
        this->insert( make_pair(key_, val_));
 
        _mutex_stats.TryUnLock();
 
        return ;
    }


    template<class K, class V>
    sfmap::map<K, V>& sfmap::map<K, V>::operator=( const map& val_ )
    {
        val_._mutex_stats.TryLock();
 
        this->std::map<K, V>::operator =(val_);
 
        val_._mutex_stats.TryUnLock();
 
        return *this;
    }
 
    template<class K, class V>
    sfmap::map<K, V>::~map()
    {
 
    }
 
    template<class K, class V>
    sfmap::map<K, V>::map( const sfmap::map<K, V>& val_ )
    {
        val_._mutex_stats.TryLock();
 
        this->std::map<K, V>::operator =(val_);
 
        val_._mutex_stats.TryUnLock();
    }
 
    template<class K, class V>
    sfmap::map<K, V>::map()
    {
 
    }
}
