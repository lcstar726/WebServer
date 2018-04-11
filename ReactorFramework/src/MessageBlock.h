#ifndef __MESSAGE_BLOCK_H__
#define __MESSAGE_BLOCK_H__

#include "Public.h"
#include "GlobalMacros.h"
#include "TimeValue.h"
#include "Allocator.h"
#include "Lock.h"

#if !defined (DEFAULT_MESSAGE_BLOCK_PRIORITY)
#define DEFAULT_MESSAGE_BLOCK_PRIORITY 0
#endif

class DataBlock;

class MessageBlock
{
    friend class DataBlock;

public:    
    typedef int tMessageType;
    typedef unsigned long tMessageFlags;
    
    enum
    {
        MB_DATA     = 0x01
        , MB_PROTO    = 0x02
        , MB_BREAK    = 0x03
        , MB_PASSFP   = 0x04
        , MB_EVENT    = 0x05
        , MB_SIG      = 0x06
        , MB_IOCTL    = 0x07
        , MB_SETOPTS  = 0x08
        , MB_IOCACK   = 0x81
        , MB_IOCNAK   = 0x82
        , MB_PCPROTO  = 0x83
        , MB_PCSIG    = 0x84
        , MB_READ     = 0x85
        , MB_FLUSH    = 0x86
        , MB_STOP     = 0x87
        , MB_START    = 0x88
        , MB_HANGUP   = 0x89
        , MB_ERROR    = 0x8a
        , MB_PCEVENT  = 0x8b
        , MB_NORMAL   = 0x00
        , MB_PRIORITY = 0x80
        , MB_USER     = 0x200
    };
    
    enum
    {
        DONT_DELETE = 01
        , USER_FLAGS = 0x1000
    };
    
    MessageBlock( AllocatorBase *msgBlkAlloc = 0 );
    
    MessageBlock( DataBlock *dataBlk
                , tMessageFlags msgFlags = 0
                , AllocatorBase *msgBlkAlloc = 0 );

    MessageBlock( const char* data
                , size_t size = 0
                , unsigned long priority = DEFAULT_MESSAGE_BLOCK_PRIORITY );
                
    MessageBlock( const MessageBlock &msgBlk, size_t align );
    
    virtual ~MessageBlock( void );

    virtual MessageBlock* Release( void );
    
    MessageBlock* GetNext( void ) const;
    void SetNext( MessageBlock *msgBlk );

    void SetReadPtr( char *newPtr );
    void SetWritePtr( char *newPtr );

    DataBlock* GetDataBlock( void ) const;
    void SetDataBlock( DataBlock *dataBlk );

    char* GetBase( void ) const;

protected:
  int InitAux (size_t size
              , tMessageType type
              , MessageBlock *cont
              , const char *data
              , AllocatorBase *allocator_strategy
              , Lock *locking_strategy
              , tMessageFlags flags
              , unsigned long priority
              , const TimeValue &execution_time
              , const TimeValue &deadline_time
              , DataBlock *db
              , AllocatorBase *dataBlkAlloc
              , AllocatorBase *msgBlkAlloc );

    int ReleaseAux( Lock *lock );

    size_t m_ReadPtr;
    size_t m_WritePtr;
    
    unsigned long m_Priority;

    // = Links to other ACE_Message_Block *s.
    // Pointer to next message block in the chain.
    MessageBlock *m_Continue;
    // Pointer to next message in the list.
    MessageBlock *m_NextMsgBlk;
    // Pointer to previous message in the list.
    MessageBlock *m_PrevMsgBlk;
    
    tMessageFlags m_Flags;
        
    DataBlock* m_DataBlock;
    
    AllocatorBase *m_MsgBlkAlloc;
    
private:
    MessageBlock( const MessageBlock &msgBlk );
    MessageBlock& operator= ( const MessageBlock &msgBlk );
};

class DataBlock
{
public:
    DataBlock( void ) { }
    
    DataBlock( size_t size
            , MessageBlock::tMessageType msgType
            , const char *msgData
            , AllocatorBase *allocStrategy
            , Lock *lockingStrategy
            , MessageBlock::tMessageFlags msgFlags
            , AllocatorBase *dataBlockAlloc );

    size_t GetSize( void ) const;
    char* GetBase( void ) const;

    DataBlock* Release( Lock *lock = 0 );

    AllocatorBase* GetAllocator( void ) const;
    Lock* GetLockingStrategy( void ) const;

protected:
    virtual DataBlock* ReleaseAux( void );

    friend class MessageBlock;
    DataBlock* ReleaseNoDelete( Lock *lock );
    
    size_t m_CurrentSize;
    size_t m_MaxSize;
    int m_ReferenceCnt;
    
    MessageBlock::tMessageFlags m_MsgFlags;
    
    char *m_Base;
    
    AllocatorBase *m_AllocStrategy;
    AllocatorBase *m_DataBlockAlloc;
    Lock *m_LockingStrategy;
    
private:
    DataBlock( const DataBlock& dataBlk );
    DataBlock& operator= ( const DataBlock& dataBlk );
};

MessageBlock::MessageBlock( AllocatorBase *msgBlkAlloc )
    : m_Flags( 0 ), m_DataBlock( 0 )
{
    if( this->InitAux( 0                    //size
                    , MB_DATA               //type
                    , 0                     //cont
                    , 0                     //data
                    , 0                     //allocator
                    , 0                     //LockingStrategy
                    , DONT_DELETE           //flags
                    , 0                     //priority
                    , TimeValue::zero       //execution
                    , TimeValue::max_time   //absolute time of deadline
                    , 0                     //data block
                    , 0                     //data_block allocator
                    , msgBlkAlloc ) == -1 )
    {
        //print error;
    }
}

MessageBlock::MessageBlock( DataBlock *dataBlk
            , tMessageFlags msgFlags
            , AllocatorBase *msgBlkAlloc )
    :m_Flags( msgFlags ), m_DataBlock( 0 )
{
    if( this->InitAux( 0                    //size
                    , MB_NORMAL             //type
                    , 0                     //cont
                    , 0                     //data
                    , 0                     //allocator
                    , 0                     //LockingStrategy
                    , DONT_DELETE           //flags
                    , 0                     //priority
                    , TimeValue::zero       //execution
                    , TimeValue::max_time   //absolute time of deadline
                    , dataBlk               //data block
                    , dataBlk->GetAllocator()  //data_block allocator
                    , msgBlkAlloc ) == -1 )
    {
        //print error;
    }
}

MessageBlock::MessageBlock( const char* data
            , size_t size
            , unsigned long priority )
    :m_Flags( 0 ), m_DataBlock( 0 )
{
    if( this->InitAux( size                    //size
                    , MB_DATA             //type
                    , 0                     //cont
                    , data                  //data
                    , 0                     //allocator
                    , 0                     //LockingStrategy
                    , DONT_DELETE           //flags
                    , priority              //priority
                    , TimeValue::zero       //execution
                    , TimeValue::max_time   //absolute time of deadline
                    , 0                     //data block
                    , 0                     //data_block allocator
                    , 0 ) == -1 )
    {
        //print error;
    }

}
            
MessageBlock::MessageBlock( const MessageBlock &msgBlk, size_t align )
{
}

MessageBlock::~MessageBlock( void )
{
    if( BIT_DISABLED( this->m_Flags, DONT_DELETE ) && this->GetDataBlock() )
    {
        this->GetDataBlock()->Release();
    }
    this->m_NextMsgBlk = 0;
    this->m_PrevMsgBlk = 0;
    this->m_Continue = 0;
}

MessageBlock* MessageBlock::Release( void )
{
    DataBlock *tmpDataBlk = this->GetDataBlock();

    int destroyDBlk = 0;

    Lock *lock = 0;

    if( this->GetDataBlock() )
    {
        lock = this->GetDataBlock()->GetLockingStrategy();
        if( lock != 0 )
        {
            GUARD_RETURN( Lock, guard, *lock, 0 );

            destroyDBlk = this->ReleaseAux( lock );
        }
        else
        {
            destroyDBlk = this->ReleaseAux( 0 );
        }
    }
    else
    {
        destroyDBlk = this->ReleaseAux( 0 );
    }
    if( destroyDBlk != 0 )
    {
        AllocatorBase *tmpAlloc = this->m_MsgBlkAlloc;
        DES_FREE( tmpDataBlk, tmpAlloc->Free, DataBlock );
    }

    return 0;
}

int MessageBlock::ReleaseAux( Lock *lock )
{
    if( this->m_Continue )
    {
        MessageBlock *mbCont = this->m_Continue;
        MessageBlock *tmp = 0;

        do
        {
            tmp = mbCont;
            mbCont = mbCont->m_Continue;
            tmp->m_Continue = 0;

            DataBlock *dblk = tmp->GetDataBlock();
            if( tmp->ReleaseAux( lock ) != 0 )
            {
                AllocatorBase *tmpAlloc = dblk->GetAllocator();
                DES_FREE( dblk, tmpAlloc->Free, DataBlock );
            }
        }
        while( mbCont );

        this->m_Continue = 0;
    }

    int result = 0;

    if( BIT_DISABLED( this->m_Flags, MessageBlock::DONT_DELETE)
        && this->GetDataBlock() )
    {
        if( 0 == this->GetDataBlock()->ReleaseNoDelete( lock ) )
        {
            result = 1;
            this->m_DataBlock = 0;
        }
    }

    if( 0 == this->m_MsgBlkAlloc )
    {
        delete this;
    }
    else
    {
        AllocatorBase *tmpAlloc = this->m_MsgBlkAlloc;
        DES_FREE( this->m_DataBlock, tmpAlloc->Free, DataBlock );
    }

    return result;
}

inline MessageBlock* MessageBlock::GetNext( void ) const
{
    return this->m_NextMsgBlk;
}
void MessageBlock::SetNext( MessageBlock *msgBlk )
{
    this->m_NextMsgBlk = msgBlk;
}

void MessageBlock::SetReadPtr( char *newPtr )
{
    this->m_ReadPtr = newPtr - this->GetBase();
}
void MessageBlock::SetWritePtr( char *newPtr )
{
    this->m_WritePtr = newPtr - this->GetBase();
}

inline DataBlock* MessageBlock::GetDataBlock( void ) const
{
    return this->m_DataBlock;
}
void MessageBlock::SetDataBlock( DataBlock *dataBlk )
{
    if( BIT_DISABLED( this->m_Flags, MessageBlock::DONT_DELETE )
        && this->m_DataBlock != 0 )
    {
        this->m_DataBlock->Release();
    }
    this->m_DataBlock = dataBlk;

    this->SetReadPtr( this->GetDataBlock()->GetBase() );
    this->SetWritePtr( this->GetDataBlock()->GetBase() );
}

inline char* MessageBlock::GetBase( void ) const
{
    return this->GetDataBlock()->GetBase();
}

int MessageBlock::InitAux (size_t size
                        , tMessageType type
                        , MessageBlock *cont
                        , const char *data
                        , AllocatorBase *allocStrategy
                        , Lock *lockingStrategy
                        , tMessageFlags flags
                        , unsigned long priority
                        , const TimeValue &executionTime
                        , const TimeValue &deadlineTime
                        , DataBlock *dataBlk
                        , AllocatorBase *dataBlkAlloc
                        , AllocatorBase *msgBlkAlloc )
{
    this->m_ReadPtr = 0;
    this->m_WritePtr = 0;
    this->m_Priority = 0;

    this->m_Continue = 0;
    this->m_NextMsgBlk = 0;
    this->m_PrevMsgBlk = 0;

    this->m_MsgBlkAlloc = msgBlkAlloc;

    if( this->m_DataBlock != 0 )
    {
        this->m_DataBlock->Release();
        this->m_DataBlock = 0;
    }

    if( 0 == dataBlk )
    {
        if( 0 == dataBlkAlloc )
        {
            ALLOCATOR_RETURN( dataBlkAlloc, AllocatorBase::GetInstance(), -1 );
        }
        NEW_MALLOC_RETURN( dataBlk
                        , static_cast<DataBlock*>(dataBlkAlloc->Malloc(sizeof(DataBlock)))
                        , DataBlock( size, type, data, allocStrategy, lockingStrategy, flags, dataBlkAlloc )
                        , -1 );
        if( dataBlk != 0 && dataBlk->GetSize() < size )
        {
            dataBlk->DataBlock::~DataBlock();
            dataBlkAlloc->Free( dataBlk );
            errno = ENOMEM;
            return -1;
        }
    }
    this->SetDataBlock( dataBlk );

    return 0;
}


DataBlock::DataBlock( size_t size
            , MessageBlock::tMessageType msgType
            , const char *msgData
            , AllocatorBase *allocStrategy
            , Lock *lockingStrategy
            , MessageBlock::tMessageFlags msgFlags
            , AllocatorBase *dataBlockAlloc )
{
}

inline size_t DataBlock::GetSize( void ) const
{
    return this->m_CurrentSize;
}

char* DataBlock::GetBase( void ) const
{
    return this->m_Base;
}

AllocatorBase* DataBlock::GetAllocator( void ) const
{
    return this->m_DataBlockAlloc;
}

Lock* DataBlock::GetLockingStrategy( void ) const
{
    return this->m_LockingStrategy;
}

DataBlock* DataBlock::ReleaseAux( void )
{
    DataBlock *result = 0;

    if( this->m_ReferenceCnt < 0 )
    {
        return 0;
    }
    --this->m_ReferenceCnt;
    if( 0 == this->m_ReferenceCnt )
    {
        result = 0;
    }
    else
    {
        result = this;
    }

    return result;
}
DataBlock* DataBlock::ReleaseNoDelete(Lock * lock)
{
    DataBlock *result = 0;
    Lock *lockToBeUsed = 0;

    if( lock != 0 )
    {
        if( lock == this->m_LockingStrategy )
        {
            lockToBeUsed = 0;
        }
        else
        {
            lockToBeUsed = this->m_LockingStrategy;
        }
        
    }
    else
    {
        lockToBeUsed = this->m_LockingStrategy;
    }

    if( lockToBeUsed != 0 )
    {
        GUARD_RETURN( Lock, guard, *lockToBeUsed, 0 );

        result = this->ReleaseAux();
    }
    else
    {
        result = this->ReleaseAux();
    }
    return result;
}
DataBlock* DataBlock::Release( Lock *lock )
{
    DataBlock *result = this->ReleaseNoDelete( lock );
    
    if( 0 == result )
    {
        DES_FREE( this
                , this->m_DataBlockAlloc->Free
                , DataBlock );
    }
    return result;
}

#endif

