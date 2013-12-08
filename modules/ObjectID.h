/*
    Copyright 2013 Marcin Wrochniak

    This file is part of Guc DB.

    Guc DB is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    Guc DB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/
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
