namespace Db {

struct ObjectID
{
    ObjectID(unsigned long long slabID, unsigned long slabInnerID)
        : slabID(slabID), slabInnerID(slabInnerID) {}

    ObjectID() {}

    ObjectID(uint64_t intRepr)
        : slabID(intRepr >> 20), slabInnerID((intRepr << 44) >> 44) {}

    operator uint64_t()
    {
        /* 20 is roughly sizeof(slabInnerID) (using sizeof operator doesn't work for bitfields) */
        return (slabID << 20) | slabInnerID;
    }

    friend bool operator==(const ObjectID a, const ObjectID b)
    {
        return (a.slabID == b.slabID) && (a.slabInnerID == b.slabInnerID);
    }

    bool isNull()
    {
        return slabID == 0 && slabInnerID == 0;
    }

    /* We'd like to sizeof(ObjectID) == 64 */
    unsigned long long slabID : 44;
    unsigned long slabInnerID : 20;
};

}
