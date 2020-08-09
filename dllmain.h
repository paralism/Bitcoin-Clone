/*Copyright 2016-2019 hyperchain.net (Hyperchain)

Distributed under the MIT software license, see the accompanying
file COPYING or?https://opensource.org/licenses/MIT.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this? software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED,? INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2011 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_MAIN_H
#define BITCOIN_MAIN_H

#include "bignum.h"
#include "net.h"
#include "key.h"
#include "script.h"
#include "db.h"

#include "node/Singleton.h"
#include "HyperChain/HyperChainSpace.h"
#include "headers/inter_public.h"

#include "bloomfilter.h"


#include "ethash/ethash.h"
#include "ethash/progpow.hpp"
#include "ethash/keccak.h"

#include "cryptocurrency.h"

#include <list>
#include <algorithm>

class CBlock;
class CBlockIndex;
class CWalletTx;
class CWallet;
class CKeyItem;
class CReserveKey;
class CWalletDB;

class CAddress;
class CInv;
class CRequestTracker;
class CNode;
class CBlockIndex;
class CBlockIndexSimplified;
class CBlockBloomFilter;
class CBlockDiskLocator;

static const unsigned int MAX_BLOCK_SIZE = 1000000;
static const unsigned int MAX_BLOCK_SIZE_GEN = MAX_BLOCK_SIZE/2;
static const int MAX_BLOCK_SIGOPS = MAX_BLOCK_SIZE/50;
static const int64 COIN = 100000000;
static const int64 CENT = 1000000;
static const int64 MIN_TX_FEE = 50000;
static const int64 MIN_RELAY_TX_FEE = 10000;
static const int64 MAX_MONEY = 210000000 * COIN;
inline bool MoneyRange(int64 nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

static const int COINBASE_MATURITY =  120;


static const int BLOCK_MATURITY =  10;

// Threshold for nLockTime: below this value it is interpreted as block number, otherwise as UNIX timestamp.
static const int LOCKTIME_THRESHOLD = 500000000; // Tue Nov  5 00:53:20 1985 UTC
#ifdef USE_UPNP
static const int fHaveUPnP = true;
#else
static const int fHaveUPnP = false;
#endif

extern CCriticalSection cs_main;
extern std::map<uint256, CBlockIndex*> mapBlockIndex;
extern map<uint256, CBlock*> mapOrphanBlocks;
extern uint256 hashGenesisBlock;
extern CBlockIndex* pindexGenesisBlock;
extern int nBestHeight;
extern CBigNum bnBestChainWork;
extern CBigNum bnBestInvalidWork;
extern uint256 hashBestChain;
extern CBlockIndex* pindexBest;

extern unsigned int nTransactionsUpdated;
extern double dHashesPerSec;
extern int64 nHPSTimerStart;
extern int64 nTimeBestReceived;
extern CCriticalSection cs_setpwalletRegistered;
extern std::set<CWallet*> setpwalletRegistered;

extern T_LOCALBLOCKADDRESS addrMaxChain;

// Settings
extern int fGenerateBitcoins;
extern int64 nTransactionFee;
extern int fLimitProcessors;
extern int nLimitProcessors;
extern int fMinimizeToTray;
extern int fMinimizeOnClose;
extern int fUseUPnP;

extern CAddress g_seedserver;
extern std::atomic<uint32_t> g_nHeightCheckPoint;
extern std::atomic<uint256> g_hashCheckPoint;

class CBlockCacheLocator;
extern CBlockCacheLocator mapBlocks;



class CReserveKey;
class CTxDB;
class CTxIndex;

void RegisterWallet(CWallet* pwalletIn);
void UnregisterWallet(CWallet* pwalletIn);
bool CheckDiskSpace(uint64 nAdditionalBytes=0);
FILE* OpenBlockFile(unsigned int nFile, unsigned int nBlockPos, const char* pszMode="rb");
FILE* AppendBlockFile(unsigned int& nFileRet);
bool LoadBlockIndex(bool fAllowNew=true);
bool LoadBlockUnChained();
void PrintBlockTree();
bool ProcessMessages(CNode* pfrom);
bool SendMessages(CNode* pto, bool fSendTrickle);
void GenerateBitcoins(bool fGenerate, CWallet* pwallet);
CBlock* CreateNewBlock(CReserveKey& reservekey);

bool CommitGenesisToConsensus(CBlock *pblock, std::string &requestid, std::string &errmsg);
bool CommitChainToConsensus(deque<CBlock>& deqblock, string &requestid, string &errmsg);

void IncrementExtraNonce(CBlock* pblock, unsigned int& nExtraNonce);
void FormatHashBuffers(CBlock* pblock, char* pmidstate, char* pdata, char* phash1);
bool CheckWork(CBlock* pblock, CWallet& wallet, CReserveKey& reservekey);
bool CheckProofOfWork(uint256 hash, unsigned int nBits);
int GetTotalBlocksEstimate();
bool IsInitialBlockDownload();
std::string GetWarnings(std::string strFor);

bool ProcessBlock(CNode* pfrom, CBlock* pblock);
bool ProcessBlock(CNode* pfrom, CBlock* pblock, T_LOCALBLOCKADDRESS* pblockaddr);
bool ProcessBlockFromAcceptedHyperBlock(CBlock* pblock, T_LOCALBLOCKADDRESS* pblockaddr);

bool GetWalletFile(CWallet* pwallet, std::string &strWalletFileOut);
bool GetBlockData(const uint256& hashBlock, CBlock& block, T_LOCALBLOCKADDRESS& addrblock);

CBlockIndex* LatestBlockIndexOnChained();

extern bool ResolveBlock(CBlock& block, const char* payload, size_t payloadlen);

template<typename T>
bool WriteSetting(const std::string& strKey, const T& value)
{
    bool fOk = false;
    BOOST_FOREACH(CWallet* pwallet, setpwalletRegistered)
    {
        std::string strWalletFile;
        if (!GetWalletFile(pwallet, strWalletFile))
            continue;
        fOk |= CWalletDB(strWalletFile).WriteSetting(strKey, value);
    }
    return fOk;
}


class CDiskTxPos
{
public:
    T_LOCALBLOCKADDRESS addr;
    //unsigned int nFile;
    //unsigned int nBlockPos;       

    unsigned int nTxPos = 0;        

    uint32_t nHeight = 0;           


    CDiskTxPos()
    {
        SetNull();
    }

    CDiskTxPos(const T_LOCALBLOCKADDRESS& addrIn, unsigned int nTxPosIn, unsigned int height):
        addr(addrIn),nTxPos(nTxPosIn), nHeight(height)
    { }
    CDiskTxPos(unsigned int nTxPosIn) : nTxPos(nTxPosIn) { }


    IMPLEMENT_SERIALIZE(

        uint32_t* hid = (uint32_t*)(&addr.hid);
        READWRITE(*hid);

        READWRITE(addr.chainnum);
        READWRITE(addr.id);
        READWRITE(addr.ns);
        READWRITE(nTxPos);
        READWRITE(nHeight);
        //READWRITE(FLATDATA(*this));
    )
    void SetNull() { nTxPos = 0; nHeight = 0; }
    bool IsNull() const { return (nTxPos == 0); }

    friend bool operator==(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return (a.addr == b.addr &&
                a.nTxPos == b.nTxPos && a.nHeight == b.nHeight);
    }

    friend bool operator!=(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        if (IsNull())
            return strprintf("null");
        else
            return strprintf("(addr=%s, nTxPos=%d height=%d)", addr.tostring().c_str(), nTxPos, nHeight);
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }
};

class CInPoint
{
public:
    CTransaction* ptx;
    unsigned int n;

    CInPoint() { SetNull(); }
    CInPoint(CTransaction* ptxIn, unsigned int nIn) { ptx = ptxIn; n = nIn; }
    void SetNull() { ptx = NULL; n = -1; }
    bool IsNull() const { return (ptx == NULL && n == -1); }
};

class COutPoint
{
public:
    uint256 hash;
    unsigned int n;

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, unsigned int nIn) { hash = hashIn; n = nIn; }
    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { hash = 0; n = -1; }
    bool IsNull() const { return (hash == 0 && n == -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        return strprintf("COutPoint(%s, %d)", hash.ToString().substr(0,10).c_str(), n);
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};

//
// An input of a transaction.  It contains the location of the previous
// transaction's output that it claims and a signature that matches the
// output's public key.
//
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    unsigned int nSequence;

    CTxIn()
    {
        nSequence = UINT_MAX;
    }

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=UINT_MAX)
    {
        prevout = prevoutIn;
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=UINT_MAX)
    {
        prevout = COutPoint(hashPrevTx, nOut);
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);
    )

    bool IsFinal() const
    {
        return (nSequence == UINT_MAX);
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.prevout   == b.prevout &&
                a.scriptSig == b.scriptSig &&
                a.nSequence == b.nSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        std::string str;
        str += strprintf("CTxIn(");
        str += prevout.ToString();
        if (prevout.IsNull())
            str += strprintf(", coinbase %s", HexStr(scriptSig).c_str());
        else
            str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24).c_str());
        if (nSequence != UINT_MAX)
            str += strprintf(", nSequence=%u", nSequence);
        str += ")";
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};

//
// An output of a transaction.  It contains the public key that the next input
// must be able to sign with to claim it.
//
class CTxOut
{
public:
    int64 nValue;
    CScript scriptPubKey;

    CTxOut()
    {
        SetNull();
    }

    CTxOut(int64 nValueIn, CScript scriptPubKeyIn)
    {
        nValue = nValueIn;
        scriptPubKey = scriptPubKeyIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nValue);
        READWRITE(scriptPubKey);
    )

    void SetNull()
    {
        nValue = -1;
        scriptPubKey.clear();
    }

    bool IsNull()
    {
        return (nValue == -1);
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
        return (a.nValue       == b.nValue &&
                a.scriptPubKey == b.scriptPubKey);
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        if (scriptPubKey.size() < 6)
            return "CTxOut(error)";

        return strprintf("CTxOut(nValue=%" PRI64d ".%08" PRI64d ", scriptPubKey=%s)", nValue / COIN, nValue % COIN, scriptPubKey.ToString().substr(0,30).c_str());
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};

//
// The basic transaction that is broadcasted on the network and contained in
// blocks.  A transaction can contain multiple inputs and outputs.
//

class CTransaction
{
public:
    int nVersion;
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    unsigned int nLockTime;


    CTransaction()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(vin);
        READWRITE(vout);
        READWRITE(nLockTime);
    )

    void SetNull()
    {
        nVersion = 1;
        vin.clear();
        vout.clear();
        nLockTime = 0;
    }

    bool IsNull() const
    {
        return (vin.empty() && vout.empty());
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }












    bool IsFinal(int nBlockHeight=0, int64 nBlockTime=0) const
    {
        // Time based nLockTime implemented in 0.1.6
        if (nLockTime == 0)
            return true;
        if (nBlockHeight == 0)
            nBlockHeight = nBestHeight;
        if (nBlockTime == 0)
            nBlockTime = GetAdjustedTime();
        if ((int64)nLockTime < (nLockTime < LOCKTIME_THRESHOLD ? (int64)nBlockHeight : nBlockTime))
            return true;
        BOOST_FOREACH(const CTxIn& txin, vin)
            if (!txin.IsFinal())
                return false;
        return true;
    }

    bool IsNewerThan(const CTransaction& old) const
    {
        if (vin.size() != old.vin.size())
            return false;
        for (int i = 0; i < vin.size(); i++)
            if (vin[i].prevout != old.vin[i].prevout)
                return false;

        bool fNewer = false;
        unsigned int nLowest = UINT_MAX;
        for (int i = 0; i < vin.size(); i++)
        {
            if (vin[i].nSequence != old.vin[i].nSequence)
            {
                if (vin[i].nSequence <= nLowest)
                {
                    fNewer = false;
                    nLowest = vin[i].nSequence;
                }
                if (old.vin[i].nSequence < nLowest)
                {
                    fNewer = true;
                    nLowest = old.vin[i].nSequence;
                }
            }
        }
        return fNewer;
    }

    bool IsCoinBase() const
    {
        return (vin.size() == 1 && vin[0].prevout.IsNull());
    }

    int GetSigOpCount() const
    {
        int n = 0;
        BOOST_FOREACH(const CTxIn& txin, vin)
            n += txin.scriptSig.GetSigOpCount();
        BOOST_FOREACH(const CTxOut& txout, vout)
            n += txout.scriptPubKey.GetSigOpCount();
        return n;
    }

    bool IsStandard() const
    {
        BOOST_FOREACH(const CTxIn& txin, vin)
            if (!txin.scriptSig.IsPushOnly())
                return ERROR_FL("nonstandard txin: %s", txin.scriptSig.ToString().c_str());
        BOOST_FOREACH(const CTxOut& txout, vout)
            if (!::IsStandard(txout.scriptPubKey))
                return ERROR_FL("nonstandard txout: %s", txout.scriptPubKey.ToString().c_str());
        return true;
    }

    int64 GetValueOut() const
    {
        int64 nValueOut = 0;
        BOOST_FOREACH(const CTxOut& txout, vout)
        {
            nValueOut += txout.nValue;
            if (!MoneyRange(txout.nValue) || !MoneyRange(nValueOut))
                throw std::runtime_error("CTransaction::GetValueOut() : value out of range");
        }
        return nValueOut;
    }

    static bool AllowFree(double dPriority)
    {
        // Large (in bytes) low-priority (new, small-coin) transactions
        // need a fee.
        return dPriority > COIN * 144 / 250;
    }

    int64 GetMinFee(unsigned int nBlockSize=1, bool fAllowFree=true, bool fForRelay=false) const
    {
        // Base fee is either MIN_TX_FEE or MIN_RELAY_TX_FEE
        int64 nBaseFee = fForRelay ? MIN_RELAY_TX_FEE : MIN_TX_FEE;

        unsigned int nBytes = ::GetSerializeSize(*this, SER_NETWORK);
        unsigned int nNewBlockSize = nBlockSize + nBytes;
        int64 nMinFee = (1 + (int64)nBytes / 1000) * nBaseFee;

        if (fAllowFree)
        {
            if (nBlockSize == 1)
            {
                // Transactions under 10K are free
                // (about 4500bc if made of 50bc inputs)
                if (nBytes < 10000)
                    nMinFee = 0;
            }
            else
            {
                // Free transaction area
                if (nNewBlockSize < 27000)
                    nMinFee = 0;
            }
        }

        // To limit dust spam, require MIN_TX_FEE/MIN_RELAY_TX_FEE if any output is less than 0.01
        if (nMinFee < nBaseFee)
            BOOST_FOREACH(const CTxOut& txout, vout)
                if (txout.nValue < CENT)
                    nMinFee = nBaseFee;

        // Raise the price as the block approaches full
        if (nBlockSize != 1 && nNewBlockSize >= MAX_BLOCK_SIZE_GEN/2)
        {
            if (nNewBlockSize >= MAX_BLOCK_SIZE_GEN)
                return MAX_MONEY;
            nMinFee *= MAX_BLOCK_SIZE_GEN / (MAX_BLOCK_SIZE_GEN - nNewBlockSize);
        }

        if (!MoneyRange(nMinFee))
            nMinFee = MAX_MONEY;
        return nMinFee;
    }

    //bool ReadFromDisk(CDiskTxPos pos, FILE** pfileRet=NULL)
    //{
    //    CAutoFile filein = OpenBlockFile(pos.nFile, 0, pfileRet ? "rb+" : "rb");
    //    if (!filein)
    //        return error("CTransaction::ReadFromDisk() : OpenBlockFile failed");

    //    // Read transaction
    //    if (fseek(filein, pos.nTxPos, SEEK_SET) != 0)
    //        return error("CTransaction::ReadFromDisk() : fseek failed");
    //    filein >> *this;

    //    // Return file pointer
    //    if (pfileRet)
    //    {
    //        if (fseek(filein, pos.nTxPos, SEEK_SET) != 0)
    //            return error("CTransaction::ReadFromDisk() : second fseek failed");
    //        *pfileRet = filein.release();
    //    }
    //    return true;
    //}

    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        return (a.nVersion  == b.nVersion &&
                a.vin       == b.vin &&
                a.vout      == b.vout &&
                a.nLockTime == b.nLockTime);
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return !(a == b);
    }


    std::string ToString() const
    {
        std::string str;
        str += strprintf("CTransaction(hash=%s, ver=%d, vin.size=%d, vout.size=%d, nLockTime=%d)\n",
            GetHash().ToString().substr(0,10).c_str(),
            nVersion,
            vin.size(),
            vout.size(),
            nLockTime);
        for (size_t i = 0; i < vin.size(); i++)
            str += "    " + vin[i].ToString() + "\n";
        for (size_t i = 0; i < vout.size(); i++)
            str += "    " + vout[i].ToString() + "\n";
        return str;
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }


    bool ReadFromDisk(CDiskTxPos pos);
    bool ReadFromDisk(CTxDB& txdb, COutPoint prevout, CTxIndex& txindexRet);
    bool ReadFromDisk(CTxDB& txdb, COutPoint prevout);
    bool ReadFromDisk(COutPoint prevout);
    bool DisconnectInputs(CTxDB& txdb);
    bool ConnectInputs(CTxDB& txdb, std::map<uint256, CTxIndex>& mapTestPool, CDiskTxPos posThisTx,
                       CBlockIndex* pindexBlock, int64& nFees, bool fBlock, bool fMiner, int64 nMinFee=0);
    bool ClientConnectInputs();
    bool CheckTransaction() const;
    bool AcceptToMemoryPool(CTxDB& txdb, bool fCheckInputs=true, bool* pfMissingInputs=NULL);
    bool AcceptToMemoryPool(bool fCheckInputs=true, bool* pfMissingInputs=NULL);

    bool AddToMemoryPoolUnchecked();
public:
    bool RemoveFromMemoryPool();
};





//
// A transaction with a merkle branch linking it to the block chain
//
class CMerkleTx : public CTransaction
{
public:
    uint256 hashBlock;
    std::vector<uint256> vMerkleBranch;
    int nIndex;

    // memory only
    mutable char fMerkleVerified;


    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }

    void Init()
    {
        hashBlock = 0;
        nIndex = -1;
        fMerkleVerified = false;
    }


    IMPLEMENT_SERIALIZE
    (
        nSerSize += SerReadWrite(s, *(CTransaction*)this, nType, nVersion, ser_action);
        nVersion = this->nVersion;
        READWRITE(hashBlock);
        READWRITE(vMerkleBranch);
        READWRITE(nIndex);
    )


    int SetMerkleBranch(const CBlock* pblock=NULL);
    int GetDepthInMainChain(int& nHeightRet) const;
    int GetDepthInMainChain() const { int nHeight; return GetDepthInMainChain(nHeight); }
    bool IsInMainChain() const { return GetDepthInMainChain() > 0; }
    int GetBlocksToMaturity() const;
    bool AcceptToMemoryPool(CTxDB& txdb, bool fCheckInputs=true);
    bool AcceptToMemoryPool();
};




//
// A txdb record that contains the disk location of a transaction and the
// locations of transactions that spend its outputs.  vSpent is really only
// used as a flag, but having the location is very helpful for debugging.
//
class CTxIndex
{
public:
    CDiskTxPos pos;
    std::vector<CDiskTxPos> vSpent;

    CTxIndex()
    {
        SetNull();
    }

    CTxIndex(const CDiskTxPos& posIn, unsigned int nOutputs)
    {
        pos = posIn;
        vSpent.resize(nOutputs);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(pos);
        READWRITE(vSpent);
    )

    void SetNull()
    {
        pos.SetNull();
        vSpent.clear();
    }

    bool IsNull()
    {
        return pos.IsNull();
    }

    friend bool operator==(const CTxIndex& a, const CTxIndex& b)
    {
        return (a.pos    == b.pos &&
                a.vSpent == b.vSpent);
    }

    friend bool operator!=(const CTxIndex& a, const CTxIndex& b)
    {
        return !(a == b);
    }
    int GetDepthInMainChain() const;
};





//
// Nodes collect new transactions into a block, hash them into a hash tree,
// and scan through nonce values to make the block's hash satisfy proof-of-work
// requirements.  When they solve the proof-of-work, they broadcast the block
// to everyone and the block is added to the block chain.  The first transaction
// in the block is a special one that creates a new coin owned by the creator
// of the block.
//
// Blocks are appended to blk0001.dat files on disk.  Their location on disk
// is indexed by CBlockIndex objects in memory.
//
class CBlock
{
public:
    // header
    int nVersion;
    uint256 hashPrevBlock; 

    uint256 hashMerkleRoot;

    uint32_t nHeight;
    uint32_t nReserved[7];
    uint32_t nTime;
    uint32_t nBits;
    uint64 nNonce;

    uint32_t nPrevHID;
    uint256 hashPrevHyperBlock;
    uint256 hashExternData = 0;         


    // network and disk
    std::vector<CTransaction> vtx;

    

    std::vector<unsigned char> nSolution;  // Equihash solution.

    // memory only
    CUInt128 ownerNodeID = CUInt128(true);
    mutable std::vector<uint256> vMerkleTree;


    CBlock()
    {
        SetNull();
    }

    CBlock(const CBlock& srcblock)
    {
        Set(srcblock);
    }

    CBlock& operator=(const CBlock& srcblock)
    {
        Set(srcblock);
        return *this;
    }


    void Set(const CBlock& srcblock)
    {
        nVersion = srcblock.nVersion;
        hashPrevBlock = srcblock.hashPrevBlock;
        hashMerkleRoot = srcblock.hashMerkleRoot;

        nHeight = srcblock.nHeight;
        memcpy(nReserved, srcblock.nReserved, sizeof(nReserved));
        nTime = srcblock.nTime;
        nBits = srcblock.nBits;
        nNonce = srcblock.nNonce;

        nPrevHID = srcblock.nPrevHID;
        hashPrevHyperBlock = srcblock.hashPrevHyperBlock;
        hashExternData = srcblock.hashExternData;

        vtx = srcblock.vtx;

        nSolution = srcblock.nSolution;

        ownerNodeID = srcblock.ownerNodeID;
        vMerkleTree = srcblock.vMerkleTree;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nVersion);
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nHeight);
        for (size_t i = 0; i < (sizeof(nReserved) / sizeof(nReserved[0])); i++) {
            READWRITE(nReserved[i]);
        }
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);

        READWRITE(nPrevHID);
        READWRITE(hashPrevHyperBlock);
        READWRITE(hashExternData);

        // ConnectBlock depends on vtx being last so it can calculate offset
        if (!(nType & (SER_GETHASH|SER_BLOCKHEADERONLY)))
            READWRITE(vtx);
        else if (fRead)
            const_cast<CBlock*>(this)->vtx.clear();

        READWRITE(nSolution);
        if (nType & (SER_NETWORK | SER_DISK)) {
            READWRITE(ownerNodeID.Lower64());
            READWRITE(ownerNodeID.High64());
        }
    )

    void SetNull()
    {
        nVersion = 1;
        hashPrevBlock = 0;
        hashMerkleRoot = 0;

        nHeight = 0;
        memset(nReserved, 0, sizeof(nReserved));
        nTime = 0;
        nBits = 0;
        nNonce = 0;
        nSolution.clear();

        nPrevHID = 0;
        hashPrevHyperBlock = 0;

        vtx.clear();
        vMerkleTree.clear();
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    void SetHyperBlockInfo();

    uint256 GetHash() const;


    ethash::hash256 GetHeaderHash() const;

    int64 GetBlockTime() const
    {
        return (int64)nTime;
    }

    int GetSigOpCount() const
    {
        int n = 0;
        BOOST_FOREACH(const CTransaction& tx, vtx)
            n += tx.GetSigOpCount();
        return n;
    }


    uint256 BuildMerkleTree() const
    {
        vMerkleTree.clear();
        BOOST_FOREACH(const CTransaction& tx, vtx)
            vMerkleTree.push_back(tx.GetHash());
        int j = 0;
        for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            for (int i = 0; i < nSize; i += 2)
            {
                int i2 = std::min(i+1, nSize-1);
                vMerkleTree.push_back(Hash(BEGIN(vMerkleTree[j+i]),  END(vMerkleTree[j+i]),
                                           BEGIN(vMerkleTree[j+i2]), END(vMerkleTree[j+i2])));
            }
            j += nSize;
        }
        return (vMerkleTree.empty() ? 0 : vMerkleTree.back());
    }

    std::vector<uint256> GetMerkleBranch(int nIndex) const
    {
        if (vMerkleTree.empty())
            BuildMerkleTree();
        std::vector<uint256> vMerkleBranch;
        int j = 0;
        for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            int i = std::min(nIndex^1, nSize-1);
            vMerkleBranch.push_back(vMerkleTree[j+i]);
            nIndex >>= 1;
            j += nSize;
        }
        return vMerkleBranch;
    }

    static uint256 CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex)
    {
        if (nIndex == -1)
            return 0;
        BOOST_FOREACH(const uint256& otherside, vMerkleBranch)
        {
            if (nIndex & 1)
                hash = Hash(BEGIN(otherside), END(otherside), BEGIN(hash), END(hash));
            else
                hash = Hash(BEGIN(hash), END(hash), BEGIN(otherside), END(otherside));
            nIndex >>= 1;
        }
        return hash;
    }


    bool WriteToDisk(unsigned int& nFileRet, unsigned int& nBlockPosRet)
    {
        // Open history file to append
        CAutoFile fileout = AppendBlockFile(nFileRet);
        if (!fileout)
            return ERROR_FL("CBlock::WriteToDisk() : AppendBlockFile failed");

        // Write index header
        unsigned int nSize = fileout.GetSerializeSize(*this);
        fileout << FLATDATA(pchMessageStart) << nSize;

        // Write block
        nBlockPosRet = ftell(fileout);
        if (nBlockPosRet == -1)
            return ERROR_FL("CBlock::WriteToDisk() : ftell failed");
        fileout << *this;

        // Flush stdio buffers and commit to disk before returning
        fflush(fileout);
        if (!IsInitialBlockDownload() || (nBestHeight+1) % 500 == 0)
        {
#ifdef __WXMSW__
            _commit(_fileno(fileout));
#else
            fsync(fileno(fileout));
#endif
        }

        return true;
    }

    

    //bool ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions=true)
    //{
    //    SetNull();

    //    // Open history file to read
    //    CAutoFile filein = OpenBlockFile(nFile, nBlockPos, "rb");
    //    if (!filein)
    //        return error("CBlock::ReadFromDisk() : OpenBlockFile failed");
    //    if (!fReadTransactions)
    //        filein.nType |= SER_BLOCKHEADERONLY;

    //    // Read block
    //    filein >> *this;

    //    // Check the header
    //    if (!CheckProofOfWork(GetHash(), nBits))
    //        return error("CBlock::ReadFromDisk() : errors in block header");

    //    return true;
    //}

    bool ReadFromDisk(const CTxIndex& txidx, bool fReadTransactions = true);

    bool ReadFromDisk(const T_LOCALBLOCKADDRESS& addr, bool fReadTransactions=true)
    {
        SetNull();

        

        CHyperChainSpace *hyperchainspace = Singleton<CHyperChainSpace, string>::getInstance();

        string payload;
        if (!hyperchainspace->GetLocalBlockPayload(addr, payload)) {
            return ERROR_FL("block(%s) isn't found in my local storage", addr.tostring().c_str());
        }

        try {
            CAutoBuffer autobuff(std::move(payload));
            

            //if (!fReadTransactions)
            //    autobuff.nType |= SER_BLOCKHEADERONLY;

            autobuff >> *this;
        }
        catch (std::ios_base::failure& e) {
            return ERROR_FL("%s", e.what());
        }

        // Check the header
        if (!CheckProofOfWork(GetHash(), nBits))
            return ERROR_FL("errors in block header");

        return true;
    }

    string ToString() const
    {
        string strResult = strprintf("\nBlock height: %u\n"
            "\tversion: %d\n"
            "\tprevHID: %u\n"
            "\tprevHHash: %s\n"
            "\thashBlock: %s ******\n"
            "\thashPrevBlock: %s\n"
            "\thashMerkleRoot: %s\n"
            "\tnTime: %u\n"
            "\tnBits: %08x\n"
            "\tnNonce: %" PRIu64 "\n"
            "\tvtx: %d\n",
            nHeight,
            nVersion,
            nPrevHID,
            hashPrevHyperBlock.ToString().c_str(),
            GetHash().ToString().c_str(),
            hashPrevBlock.ToString().c_str(),
            hashMerkleRoot.ToString().c_str(),
            nTime, nBits, nNonce,
            vtx.size());

        for (int i = 0; i < vtx.size(); i++) {
            strResult += "\t";
            strResult += vtx[i].ToString();
        }
        strResult += "\tvMerkleTree: ";
        for (int i = 0; i < vMerkleTree.size(); i++)
            strResult += vMerkleTree[i].ToString().substr(0, 10);

        strResult += "\n";

        return strResult;
    }

    void print() const
    {
        printf(ToString().c_str());
    }


    bool DisconnectBlock(CTxDB& txdb, CBlockIndex* pindex);
    bool ConnectBlock(CTxDB& txdb, CBlockIndex* pindex);
    bool ReadFromDisk(const CBlockIndex* pindex, bool fReadTransactions=true);
    bool ReadFromDisk(const CBlockIndexSimplified* pindex);
    bool SetBestChain(CTxDB& txdb, CBlockIndex* pindexNew);

    

    bool AddToBlockIndex(unsigned int nFile, unsigned int nBlockPos);
    bool AddToBlockIndex(const T_LOCALBLOCKADDRESS& addr);
    bool UpdateToBlockIndex(CBlockIndex* pIndex, const T_LOCALBLOCKADDRESS& addr);

    bool IsMine() const;

    

    int CheckHyperBlockConsistence(CNode* pfrom) const;
    bool CheckExternalData() const;
    bool IsLastestHyperBlockMatched() const;
    bool CheckBlock() const;
    void GetBlock(const T_LOCALBLOCKADDRESS& addr) const;

    bool AcceptBlock();

    bool AddToMemoryPool();
    bool RemoveFromMemoryPool();
    bool ReadFromMemoryPool(uint256 nBlockHash);

	bool CheckTrans();
    bool CheckProgPow() const;

};

/**
 * Custom serializer for CBlockHeader that omits the nonce and solution, for use
 * as input to Equihash.
 */

class CEquihashInput : private CBlock
{
public:
    CEquihashInput(const CBlock &header)
    {
        CBlock::SetNull();
        *((CBlock*)this) = header;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nHeight);
        for (size_t i = 0; i < (sizeof(nReserved) / sizeof(nReserved[0])); i++) {
            READWRITE(nReserved[i]);
        }

        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nPrevHID);
        READWRITE(hashPrevHyperBlock);
        READWRITE(hashExternData);
    )

};

//
// The block chain is a tree shaped structure starting with the
// genesis block at the root, with each block potentially having multiple
// candidates to be the next block.  pprev and pnext link a path through the
// main/longest chain.  A blockindex may have multiple pprev pointing back
// to it, but pnext will only point forward to the longest branch, or will
// be null if the block is not part of the longest chain.
//
class CBlockIndex
{
public:
    const uint256* phashBlock;  

    CBlockIndex* pprev;
    CBlockIndex* pnext;
    //unsigned int nFile;     

    //unsigned int nBlockPos; 

    int nHeight;
    CBigNum bnChainWork;    


    T_LOCALBLOCKADDRESS addr; 


    // block header
    int nVersion;
    uint256 hashMerkleRoot;

    unsigned int nTime;
    unsigned int nBits;
    uint64 nNonce;

    std::vector<unsigned char> nSolution;

    uint32 nPrevHID;
    uint256 hashPrevHyperBlock;

    uint256 hashExternData = 0;                 

    CUInt128 ownerNodeID = CUInt128(true);

    CBlockIndex()
    {
        phashBlock = NULL;
        pprev = NULL;
        pnext = NULL;
        nHeight = 0;
        bnChainWork = 0;

        nVersion       = 0;
        hashMerkleRoot = 0;

        nTime          = 0;
        nBits          = 0;
        nNonce         = 0;
        nSolution.clear();
        nPrevHID = 0;
        hashPrevHyperBlock = 0;

    }

    CBlockIndex(const T_LOCALBLOCKADDRESS& addrIn, const CBlock& block)
    {
        phashBlock = NULL;
        pprev = NULL;
        pnext = NULL;
        bnChainWork = 0;

        Set(addrIn, block);
    }

    CBlockIndex(unsigned int nFileIn, unsigned int nBlockPosIn, CBlock& block)
    {
        phashBlock = NULL;
        pprev = NULL;
        pnext = NULL;
        nHeight = block.nHeight;
        bnChainWork = 0;

        nVersion       = block.nVersion;
        hashMerkleRoot = block.hashMerkleRoot;

        nTime          = block.nTime;
        nBits          = block.nBits;
        nNonce         = block.nNonce;
        nSolution = block.nSolution;
        nPrevHID = block.nPrevHID;
        hashPrevHyperBlock = block.hashPrevHyperBlock;
        hashExternData = block.hashExternData;
        ownerNodeID = block.ownerNodeID;
    }

    void Set(const T_LOCALBLOCKADDRESS& addrIn, const CBlock& block)
    {
        nHeight = block.nHeight;
        addr = addrIn;

        nVersion = block.nVersion;
        hashMerkleRoot = block.hashMerkleRoot;

        nTime = block.nTime;
        nBits = block.nBits;
        nNonce = block.nNonce;
        nSolution = block.nSolution;
        nPrevHID = block.nPrevHID;
        hashPrevHyperBlock = block.hashPrevHyperBlock;
        hashExternData = block.hashExternData;
        ownerNodeID = block.ownerNodeID;
    }

    CBlock GetBlockHeader() const
    {
        CBlock block;
        block.nVersion       = nVersion;
        if (pprev)
            block.hashPrevBlock = pprev->GetBlockHash();
        block.hashMerkleRoot = hashMerkleRoot;

        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        block.nSolution = nSolution;
        block.nPrevHID = nPrevHID;
        block.hashPrevHyperBlock = hashPrevHyperBlock;
        block.hashExternData = hashExternData;
        block.ownerNodeID = ownerNodeID;

        return block;
    }

    uint256 GetBlockHash() const
    {
        return *phashBlock;
    }

    int64 GetBlockTime() const
    {
        return (int64)nTime;
    }

    CBigNum GetBlockWork() const
    {
        CBigNum bnTarget;
        bnTarget.SetCompact(nBits);
        if (bnTarget <= 0)
            return 0;
        return (CBigNum(1)<<256) / (bnTarget+1);
    }

    bool IsInMainChain() const
    {
        return (pnext || this == pindexBest);
    }

    bool CheckIndex() const
    {
        return CheckProofOfWork(GetBlockHash(), nBits);
    }

    enum { nMedianTimeSpan=11 };

    int64 GetMedianTimePast() const
    {
        int64 pmedian[nMedianTimeSpan];
        int64* pbegin = &pmedian[nMedianTimeSpan];
        int64* pend = &pmedian[nMedianTimeSpan];

        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan && pindex; i++, pindex = pindex->pprev)
            *(--pbegin) = pindex->GetBlockTime();

        std::sort(pbegin, pend);
        return pbegin[(pend - pbegin)/2];
    }

    int64 GetMedianTime() const
    {
        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan/2; i++)
        {
            if (!pindex->pnext)
                return GetBlockTime();
            pindex = pindex->pnext;
        }
        return pindex->GetMedianTimePast();
    }

    std::string ToString() const
    {
        return strprintf("CBlockIndex: \n"
            "\tHeight=%d"
            "\tAddr=%s\n"
            "\tPrevHID: %d\n"
            "\thashPrevHyperBlock=%s\n"
            "\tmyself=%08x"
            "\tnprev=%08x"
            "\tpnext=%08x\n"
            "\tnBit=%08x"
            "\tnTime=%d\n"
            "\tnNonce=%" PRI64u "\n"
            "\tmerkle=%s\n"
            "\thashBlock=%s ******\n"
            "\thashPrevBlock=%s\n"
            "\thashNextBlock=%s\n",
            nHeight,
            addr.tostring().c_str(),
            nPrevHID,hashPrevHyperBlock.ToString().c_str(),this,
            pprev, pnext, nBits, nTime, nNonce,
            hashMerkleRoot.ToString().c_str(),
            phashBlock ? (phashBlock->ToString().c_str()) : "null",
            pprev ? (pprev->GetBlockHash().ToString().c_str()):"null",
            pnext ? (pnext->GetBlockHash().ToString().c_str()):"null");
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }

    bool operator<(const CBlockIndex &st) const
    {
        return (addr < st.addr);
    }
    bool operator>=(const CBlockIndex &st) const
    {
        return (addr >= st.addr);
    }
};

class CBlockIndexSimplified
{
public:
    const uint256* phashBlock = nullptr;  

    CBlockIndexSimplified* pprev = nullptr;
    CBlockIndexSimplified* pnext = nullptr;
    int nHeight = -1;

    T_LOCALBLOCKADDRESS addr;

public:
    void Set(const T_LOCALBLOCKADDRESS& addrIn, const CBlock& block)
    {
        addr = addrIn;
        nHeight = block.nHeight;
    }

    uint256 GetBlockHash() const
    {
        return *phashBlock;
    }

    std::string ToString() const
    {
        return strprintf("CBlockIndexSimplified: \n"
            "\tHeight=%d"
            "\tAddr=%s\n"
            "\thashBlock=%s ******\n"
            "\thashPrevBlock=%s\n"
            "\thashNextBlock=%s\n",
            nHeight,
            addr.tostring().c_str(),
            phashBlock ? (phashBlock->ToString().c_str()) : "null",
            pprev ? (pprev->GetBlockHash().ToString().c_str()) : "null",
            pnext ? (pnext->GetBlockHash().ToString().c_str()) : "null");
    }

   };


//
// Used to marshal pointers into hashes for db storage.
//
class CDiskBlockIndex : public CBlockIndex
{
public:
    uint256 hashPrev;
    uint256 hashNext;

    CDiskBlockIndex()
    {
        hashPrev = 0;
        hashNext = 0;
    }

    explicit CDiskBlockIndex(CBlockIndex* pindex) : CBlockIndex(*pindex)
    {
        hashPrev = (pprev ? pprev->GetBlockHash() : 0);
        hashNext = (pnext ? pnext->GetBlockHash() : 0);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);

        READWRITE(hashNext);

		READWRITE(nHeight);


        uint32_t* hid = (uint32_t*)(&addr.hid);
        READWRITE(*hid);

        READWRITE(addr.chainnum);
        READWRITE(addr.id);
        READWRITE(addr.ns);

        // block header
        READWRITE(this->nVersion);
        READWRITE(hashPrev);
        READWRITE(hashMerkleRoot);

        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        READWRITE(nSolution);
        READWRITE(nPrevHID);
        READWRITE(hashPrevHyperBlock);
        READWRITE(hashExternData);

        READWRITE(ownerNodeID.Lower64());
        READWRITE(ownerNodeID.High64());

    )

    uint256 GetBlockHash() const
    {
        CBlock block;
        block.nVersion        = nVersion;
        block.hashPrevBlock   = hashPrev;
        block.hashMerkleRoot  = hashMerkleRoot;
        block.nHeight         = nHeight;

        block.nTime           = nTime;
        block.nBits           = nBits;
        block.nNonce          = nNonce;
        block.nPrevHID        = nPrevHID;
        block.nSolution       = nSolution;
        block.hashPrevHyperBlock = hashPrevHyperBlock;
        block.nNonce          = nNonce;
        block.hashExternData = hashExternData;

        return block.GetHash();
    }

    std::string ToString() const
    {
        std::string str = "CDiskBlockIndex(";
        str += CBlockIndex::ToString();
        str += strprintf("\n                hashBlock=%s, hashPrev=%s, hashNext=%s)",
            GetBlockHash().ToString().c_str(),
            hashPrev.ToString().substr(0,20).c_str(),
            hashNext.ToString().substr(0,20).c_str());
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};


//
// Describes a place in the block chain to another node such that if the
// other node doesn't have the same branch, it can find a recent common trunk.
// The further back it is, the further before the fork it may be.
//
class CBlockLocator
{
protected:
    std::vector<uint256> vHave;
public:

    CBlockLocator()
    {
    }

    explicit CBlockLocator(const CBlockIndex* pindex)
    {
        Set(pindex);
    }

    explicit CBlockLocator(uint256 hashBlock)
    {
        std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);
        if (mi != mapBlockIndex.end())
            Set((*mi).second);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    )

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull()
    {
        return vHave.empty();
    }

    void Set(const CBlockIndex* pindex)
    {
        vHave.clear();
        int nStep = 1;
        while (pindex)
        {
            vHave.push_back(pindex->GetBlockHash());

            // Exponentially larger steps back
            for (int i = 0; pindex && i < nStep; i++)
                pindex = pindex->pprev;
            if (vHave.size() > 10)
                nStep *= 2;
        }
        vHave.push_back(hashGenesisBlock);
    }

    int GetDistanceBack()
    {
        // Retrace how far back it was in the sender's branch
        int nDistance = 0;
        int nStep = 1;
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return nDistance;
            }
            nDistance += nStep;
            if (nDistance > 10)
                nStep *= 2;
        }
        return nDistance;
    }

    CBlockIndex* GetBlockIndex()
    {
        // Find the first block the caller has in the main chain
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return pindex;
            }
        }
        return pindexGenesisBlock;
    }

    uint256 GetBlockHash()
    {
        // Find the first block the caller has in the main chain
        BOOST_FOREACH(const uint256& hash, vHave)
        {
            std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return hash;
            }
        }
        return hashGenesisBlock;
    }

    int GetHeight()
    {
        CBlockIndex* pindex = GetBlockIndex();
        if (!pindex)
            return 0;
        return pindex->nHeight;
    }
};

//
// Alerts are for notifying old versions if they become too obsolete and
// need to upgrade.  The message is displayed in the status bar.
// Alert messages are broadcast as a vector of signed data.  Unserializing may
// not read the entire buffer if the alert is for a newer version, but older
// versions can still relay the original data.
//
class CUnsignedAlert
{
public:
    int nVersion;
    int64 nRelayUntil;      // when newer nodes stop relaying to newer nodes
    int64 nExpiration;
    int nID;
    int nCancel;
    std::set<int> setCancel;
    int nMinVer;            // lowest version inclusive
    int nMaxVer;            // highest version inclusive
    std::set<std::string> setSubVer;  // empty matches all
    int nPriority;

    // Actions
    std::string strComment;
    std::string strStatusBar;
    std::string strReserved;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nRelayUntil);
        READWRITE(nExpiration);
        READWRITE(nID);
        READWRITE(nCancel);
        READWRITE(setCancel);
        READWRITE(nMinVer);
        READWRITE(nMaxVer);
        READWRITE(setSubVer);
        READWRITE(nPriority);

        READWRITE(strComment);
        READWRITE(strStatusBar);
        READWRITE(strReserved);
    )

    void SetNull()
    {
        nVersion = 1;
        nRelayUntil = 0;
        nExpiration = 0;
        nID = 0;
        nCancel = 0;
        setCancel.clear();
        nMinVer = 0;
        nMaxVer = 0;
        setSubVer.clear();
        nPriority = 0;

        strComment.clear();
        strStatusBar.clear();
        strReserved.clear();
    }

    std::string ToString() const
    {
        std::string strSetCancel;
        BOOST_FOREACH(int n, setCancel)
            strSetCancel += strprintf("%d ", n);
        std::string strSetSubVer;
        BOOST_FOREACH(std::string str, setSubVer)
            strSetSubVer += "\"" + str + "\" ";
        return strprintf(
                "CAlert(\n"
                "    nVersion     = %d\n"

                "    nRelayUntil  = %" PRI64d "\n"
                "    nExpiration  = %" PRI64d "\n"
                "    nID          = %d\n"
                "    nCancel      = %d\n"
                "    setCancel    = %s\n"
                "    nMinVer      = %d\n"
                "    nMaxVer      = %d\n"
                "    setSubVer    = %s\n"
                "    nPriority    = %d\n"
                "    strComment   = \"%s\"\n"
                "    strStatusBar = \"%s\"\n"
                ")\n",
            nVersion,
            nRelayUntil,
            nExpiration,
            nID,
            nCancel,
            strSetCancel.c_str(),
            nMinVer,
            nMaxVer,
            strSetSubVer.c_str(),
            nPriority,
            strComment.c_str(),
            strStatusBar.c_str());
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }
};

class CAlert : public CUnsignedAlert
{
public:
    std::vector<unsigned char> vchMsg;
    std::vector<unsigned char> vchSig;

    CAlert()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(vchMsg);
        READWRITE(vchSig);
    )

    void SetNull()
    {
        CUnsignedAlert::SetNull();
        vchMsg.clear();
        vchSig.clear();
    }

    bool IsNull() const
    {
        return (nExpiration == 0);
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    bool IsInEffect() const
    {
        return (GetAdjustedTime() < nExpiration);
    }

    bool Cancels(const CAlert& alert) const
    {
        if (!IsInEffect())
            return false; // this was a no-op before 31403
        return (alert.nID <= nCancel || setCancel.count(alert.nID));
    }

    bool AppliesTo(int nVersion, std::string strSubVerIn) const
    {
        return (IsInEffect() &&
                nMinVer <= nVersion && nVersion <= nMaxVer &&
                (setSubVer.empty() || setSubVer.count(strSubVerIn)));
    }

    bool AppliesToMe() const
    {
        return AppliesTo(VERSION, ::pszSubVer);
    }

    bool RelayTo(CNode* pnode) const
    {
        if (!IsInEffect())
            return false;
        // returns true if wasn't already contained in the set
        if (pnode->setKnown.insert(GetHash()).second)
        {
            if (AppliesTo(pnode->nVersion, pnode->strSubVer) ||
                AppliesToMe() ||
                GetAdjustedTime() < nRelayUntil)
            {
                pnode->PushMessage("alert", *this);
                return true;
            }
        }
        return false;
    }

    bool CheckSignature()
    {
        CKey key;
        if (!key.SetPubKey(ParseHex("04fc9702847840aaf195de8442ebecedf5b095cdbb9bc716bda9110971b28a49e0ead8564ff0db22209e0374782c093bb899692d524e9d6a6956e7c5ecbcd68284")))
            return ERROR_FL("CAlert::CheckSignature() : SetPubKey failed");
        if (!key.Verify(Hash(vchMsg.begin(), vchMsg.end()), vchSig))
            return ERROR_FL("CAlert::CheckSignature() : verify signature failed");

        // Now unserialize the data
        CDataStream sMsg(vchMsg);
        sMsg >> *(CUnsignedAlert*)this;
        return true;
    }

    bool ProcessAlert();
};

class BlockCheckPoint
{
public:
    BlockCheckPoint() = default;
    BlockCheckPoint(const BlockCheckPoint&) = delete;
    BlockCheckPoint& operator =(const BlockCheckPoint&) = delete;

    void Get(uint32_t& nHeight, uint256& hashblock)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        nHeight = _nHeightCheckPoint;
        hashblock = _hashCheckPoint;
    }

    void Set(uint32_t nHeight, const uint256& hashblock)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _nHeightCheckPoint = nHeight;
        _hashCheckPoint = hashblock;
    }

private:
    std::mutex _mutex;
    uint32_t _nHeightCheckPoint = 0;
    uint256 _hashCheckPoint = 0;
};


class LatestHyperBlock {

public:
    static void Sync()
    {
        CHyperChainSpace* hyperchainspace = Singleton<CHyperChainSpace, string>::getInstance();
        uint64 hid;
        T_SHA256 thhash;
        hyperchainspace->GetLatestHyperBlockIDAndHash(hid, thhash);
        CRITICAL_BLOCK(_cs_latestHyperBlock)
        {
            _hid = hid;
            _hhash = uint256S(thhash.toHexString());
        }
    }

    static void CompareAndUpdate(uint32_t hid, const T_SHA256& thhash, bool isLatest)
    {
        CRITICAL_BLOCK(_cs_latestHyperBlock)
        {
            

            if (isLatest || _hid < hid) {
                _hid = hid;
                _hhash = uint256S(thhash.toHexString());
            }
        }
    }

    static uint32_t GetHID(uint256* hhash = nullptr)
    {
        CRITICAL_BLOCK(_cs_latestHyperBlock)
        {
            if (hhash) {
                *hhash = _hhash;
            }
            return _hid;
        }
    }
private:
    static uint32_t _hid;
    static uint256 _hhash;
    static CCriticalSection _cs_latestHyperBlock;
};


class BLOCKTRIPLEADDRESS
{
public:
    uint32 hid = 0;            

    uint16 chainnum = 0;
    uint16 id = 0;

public:
    BLOCKTRIPLEADDRESS() {}

    BLOCKTRIPLEADDRESS(const T_LOCALBLOCKADDRESS& addr)
    {
        hid = addr.hid;
        chainnum = addr.chainnum;
        id = addr.id;
    }
    BLOCKTRIPLEADDRESS(const BLOCKTRIPLEADDRESS& addr)
    {
        hid = addr.hid;
        chainnum = addr.chainnum;
        id = addr.id;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(hid);
        READWRITE(chainnum);
        READWRITE(id);
    )

    T_LOCALBLOCKADDRESS ToAddr() const
    {
        T_LOCALBLOCKADDRESS addr;
        addr.hid = hid;
        addr.chainnum = chainnum;
        addr.id = id;
        return addr;
    }

    string ToString() const
    {
        return strprintf("[%d,%d,%d]", hid,chainnum,id);
    }
};



class LatestParaBlock {

public:
    static void Load();
    static void CompareAndUpdate(const vector<BLOCKTRIPLEADDRESS>& vecAddrIn, const vector<CBlock>& vecBlockIn, bool isLatest);

    static CBlockIndexSimplified* Get()
    {
        return _pindexLatest;
    }

    static uint256 GetRootHash()
    {
        if (_pindexLatestRoot && _pindexLatestRoot->pnext) {
            return *_pindexLatestRoot->pnext->phashBlock;
        }
        return 0;
    }

    static uint256 GetBackSearchHash()
    {
        if (_pindexLatestRoot) {
            return *_pindexLatestRoot->phashBlock;
        }
        return 0;
    }

    static uint32 GetBackSearchHeight()
    {
        if (_pindexLatestRoot && _pindexLatestRoot->pnext &&  _pindexLatestRoot->pnext->nHeight >= 1) {
            return _pindexLatestRoot->pnext->nHeight - 1;
        }
        return 0;
    }

    static string GetMemoryInfo();

    

    static void Switch();
    static bool IsOnChain();
    static bool IsLackingBlock();

    static bool Count(const uint256& hastblock);

    static void AddBlockTripleAddress(const uint256& hastblock, const BLOCKTRIPLEADDRESS& tripleaddr);

    static bool GetBlock(const uint256& hastblock, CBlock& block, BLOCKTRIPLEADDRESS& tripleaddr);

private:

    static bool LoadLatestBlock();
    static void SetBestIndex(CBlockIndexSimplified* pIndex)
    {
        _pindexLatest = pIndex;
    }

    static CBlockIndexSimplified* AddBlockIndex(const T_LOCALBLOCKADDRESS& addrIn, const CBlock& block);
    static CBlockIndexSimplified* InsertBlockIndex(uint256 hash);

    static void PullingPrevBlocks();

private:
    

    static CBlockIndexSimplified* _pindexLatest;


    static map<uint256, CBlockIndexSimplified*> _mapBlockIndexLatest;
    static CBlockDiskLocator _mapBlockAddressOnDisk;

    static CBlockIndexSimplified* _pindexLatestRoot;
};


extern BlockCheckPoint g_blockChckPnt;

class MiningCondition
{
public:
    MiningCondition() = default;
    MiningCondition(const MiningCondition&) = delete;
    MiningCondition& operator =(const MiningCondition&) = delete;

    typedef struct BackTrackingProgress
    {
        int nLatestBlockHeight = 0;
        std::string strLatestBlockTripleAddr;
        int nBackTrackingBlockHeight = 0;
        std::string strBackTrackingBlockHash;

    } BackTrackingProgress;

    bool EvaluateIsAllowed(bool NeighborIsMust = true) {

        CHyperChainSpace* hyperchainspace = Singleton<CHyperChainSpace, string>::getInstance();

        CRITICAL_BLOCK(cs_main)
            CRITICAL_BLOCK(_cs_miningstatus)
            {
                if (!hyperchainspace->IsLatestHyperBlockReady()) {
                    _eStatusCode = miningstatuscode::HyperBlockNotReady;
                    return false;
                }
                else if (NeighborIsMust && vNodes.empty()) {
                    _eStatusCode = miningstatuscode::NoAnyNeighbor;
                    return false;
                }
                //else if (IsInitialBlockDownload()) {
                //    _reason += "Initial Block is downloading";
                //    return false;
                //}
                else if (!g_cryptoCurrency.CheckGenesisBlock()) {
                    _eStatusCode = miningstatuscode::InvalidGenesisBlock;
                    return false;
                }
                else if (!g_cryptoCurrency.AllowMining()) {
                    _eStatusCode = miningstatuscode::MiningSettingClosed;
                    return false;
                }

                if (!LatestParaBlock::IsOnChain()) {
                    if (LatestParaBlock::IsLackingBlock()) {

                        CBlockIndexSimplified* pIndex = LatestParaBlock::Get();

                        _backTrackingProgress.nLatestBlockHeight = pIndex->nHeight;
                        _backTrackingProgress.strLatestBlockTripleAddr = pIndex->addr.tostring().c_str();
                        _backTrackingProgress.nBackTrackingBlockHeight = LatestParaBlock::GetBackSearchHeight();
                        _backTrackingProgress.strBackTrackingBlockHash = LatestParaBlock::GetBackSearchHash().ToPreViewString().c_str();
                        _eStatusCode = miningstatuscode::ChainIncomplete;
                    }
                    else {
                        

                        _eStatusCode = miningstatuscode::Switching;
                        LatestParaBlock::Switch();
                    }
                    return false;
                }

                string reason;
                if (IsTooFar(reason)) {
                    return false;
                }
                _eStatusCode = miningstatuscode::Mining;
            }

        return true;
    }

    bool IsMining()
    {
        CRITICAL_BLOCK(_cs_miningstatus)
            return  _eStatusCode > miningstatuscode::Switching;
    }

    string GetMiningStatus(bool* isAllowed) {

        if(isAllowed)
            *isAllowed = IsMining();

        return StatusCodeToReason();
    }

    int MiningStatusCode() const { return (int)(_eStatusCode); }
    bool IsBackTracking() const { return (_eStatusCode == miningstatuscode::ChainIncomplete); }
    bool IsSwitching() const { return (_eStatusCode == miningstatuscode::Switching); }

    BackTrackingProgress GetBackTrackingProcess() const { return _backTrackingProgress; }

private:
    bool IsTooFar(std::string& reason)
    {
        uint32_t ncount = 0;

        char szReason[1024] = { 0 };

        CRITICAL_BLOCK(cs_main)
        {
            CBlockIndexSimplified* pIndex = LatestParaBlock::Get();
            uint256 hash = pIndex->GetBlockHash();

            CBlockIndex* p = pindexBest;
            while (p && !p->addr.isValid() && p->GetBlockHash() != hash) {
                ncount++;
                p = p->pprev;
            }

            if (ncount > 40) {
                _eStatusCode = miningstatuscode::ManyBlocksNonChained;
                return true;
            }

            if (g_seedserver.IsValid()) {

                CBlockIndex* p = pindexBest;
                uint32_t height;
                uint256 hash;
                g_blockChckPnt.Get(height, hash);
                if (height == 0) {
                    

                    _eStatusCode = miningstatuscode::MiningWithWarning1;
                    return false;
                }
                

                if (height > 0) {
                    if (p->nHeight < height) {
                        

                        _eStatusCode = miningstatuscode::MiningWithWarning2;
                        return false;
                    }

                    while (p && p->nHeight > height) {
                        p = p->pprev;
                    }

                    if (p->GetBlockHash() != hash) {
                        

                        _eStatusCode = miningstatuscode::MiningWithWarning3;
                        return false;
                    }
                }
            }
        }
        return false;
    }

    string StatusCodeToReason()
    {
        string rs;

        if (_eStatusCode == miningstatuscode::ChainIncomplete) {
            return strprintf("The chain is incomplete, latest block height: %u(%s), backtracking block: %u(hash: %s)",
                _backTrackingProgress.nLatestBlockHeight, _backTrackingProgress.strLatestBlockTripleAddr.c_str(),
                _backTrackingProgress.nBackTrackingBlockHeight, _backTrackingProgress.strBackTrackingBlockHash.c_str());
        }
        return _mapStatusDescription.at(_eStatusCode);
    }

private:
    CCriticalSection _cs_miningstatus;

    enum class miningstatuscode : char {
        Mining = 1,
        MiningWithWarning1 = 2,
        MiningWithWarning2 = 3,
        MiningWithWarning3 = 4,

        Switching = 0,

        GenDisabled = -1,
        HyperBlockNotReady = -2,
        NoAnyNeighbor = -3,
        InvalidGenesisBlock = -4,
        MiningSettingClosed= -5,
        ManyBlocksNonChained= -6,
        ChainIncomplete = -7,
    };

    miningstatuscode _eStatusCode = miningstatuscode::GenDisabled;

    const map<miningstatuscode, string> _mapStatusDescription = {
        {miningstatuscode::Mining,              "Mining"},
        {miningstatuscode::MiningWithWarning1,  "Warning: Seed server's block information is unknown"},
        {miningstatuscode::MiningWithWarning2,  "Warning: Block height less than seed server's"},
        {miningstatuscode::MiningWithWarning3,  "Warning: Block hash different from seed server's"},
        {miningstatuscode::Switching,           "Switching to the best chain"},
        {miningstatuscode::GenDisabled,         "Specify \"-gen\" option to enable"},
        {miningstatuscode::HyperBlockNotReady,  "My latest hyper block isn't ready"},
        {miningstatuscode::NoAnyNeighbor,       "No any neighbor"},
        {miningstatuscode::InvalidGenesisBlock, "Genesis block error"},
        {miningstatuscode::MiningSettingClosed, "Coin's mining setting is closed"},
        {miningstatuscode::MiningSettingClosed, "More than 40 blocks is non-chained"},
        {miningstatuscode::ChainIncomplete,     "The chain is incomplete"},
    };

    BackTrackingProgress _backTrackingProgress;
};


class CBlockBloomFilter
{
public:
    CBlockBloomFilter();
    virtual ~CBlockBloomFilter() {};

    bool contain(const uint256& hashBlock)
    {
        return _filter.contain((char*)hashBlock.begin(), 32);
    }

    bool insert(const uint256& hashBlock)
    {
        _filter.insert((char*)hashBlock.begin(), 32);
        return true;
    }

    void clear()
    {
        _filter.clear();
    }

protected:
    BloomFilter _filter;
};

class CBlockCacheLocator
{
public:
    CBlockCacheLocator() {}
    ~CBlockCacheLocator() {}

    bool contain(const uint256& hashBlock);

    bool insert(const uint256& hashBlock)
    {
        return _filterBlock.insert(hashBlock);
    }

    bool insert(const uint256& hashBlock, const CBlock& blk);

    void clear();

    

    bool erase(const uint256& hashBlock);

    const CBlock& operator[](const uint256& hashBlock);

private:
    const size_t _capacity = 200;
    CBlockBloomFilter _filterBlock;

    std::map<uint256, CBlock> _mapBlock;
    std::map<int64, uint256> _mapTmJoined;
};

class CBlockDiskLocator
{
public:
    CBlockDiskLocator() {}
    ~CBlockDiskLocator() {}

    bool contain(const uint256& hashBlock);

    bool insert(const uint256& hashBlock)
    {
        return _filterBlock.insert(hashBlock);
    }

    size_t size()
    {
        return _sizeInserted;
    }

    bool insert(CBlockTripleAddressDB& btadb, const uint256& hashBlock, const BLOCKTRIPLEADDRESS& addr);
    bool insert(const uint256& hashBlock, const BLOCKTRIPLEADDRESS& addr);
    void clear();

      

    bool erase(const uint256& hashBlock);

    const BLOCKTRIPLEADDRESS& operator[](const uint256& hashBlock);

private:

    const size_t _capacity = 3000;

    size_t _sizeInserted = 0;
    CBlockBloomFilter _filterBlock;

    std::map<uint256, BLOCKTRIPLEADDRESS> _mapBlockTripleAddr;
    std::map<int64, uint256> _mapTmJoined;

    std::set<uint256> _setRemoved;

};


#endif
