/*
 * $Id: gport.h,v 1.71 2013/08/21 15:29:22 liortole Exp $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * This file defines gport (generic port) parameters. The gport is useful
 * for specifying the following types of identifiers:
 *   LOCAL      :  port on the local unit
 *   MODPORT    :  {module ID, port} pair
 *   TRUNK      :  trunk ID
 *   LOCAL_CPU  :  identifier indicating local CPU destination
 *   MIRROR     :  Mirror (modport & encapsulation) for mirrored traffic.
 *   VLAN_PORT  :  VLAN virtual-port
 *
 * This header requires that the uint32 type be defined before inclusion.
 * Using <sal/types.h> is the simplest (and recommended) way of doing this.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define gport parameters.
 *
 * The following macros are made available.  All have names starting
 * with _SHR_, which have been elided from this list:
 *
 * Constants or Expressions suitable for assignment:
 * GPORT_NONE                   gport initializer
 * GPORT_INVALID                invalid gport identifier
 * GPORT_LOCAL_CPU              local CPU gport identifier
 *
 * Predicates: (return 0 or 1, suitable for using in if statements)
 * GPORT_IS_SET                 is the gport set?
 * GPORT_IS_LOCAL               is the gport a local port type?
 * GPORT_IS_MODPORT             is the gport a modid/port type?
 * GPORT_IS_TRUNK               is the gport a trunk type?
 * GPORT_IS_LOCAL_CPU           is the gport a local CPU type?
 * GPORT_IS_MIRROR              is the gport a mirror destination type?
 * GPORT_IS_VLAN_PORT           is the gport a VLAN port type?
 *
 * Statements: (cannot be used as a predicate)
 * GPORT_LOCAL_SET              set a gport local port type and value
 * GPORT_LOCAL_GET              get a port value from a local gport
 * GPORT_LOCAL_CPU_SET          set a gport local CPU port type and value
 * GPORT_LOCAL_CPU_GET          get a CPU port value from a local gport
 * GPORT_MODPORT_SET            set a gport modid/port type and values
 * GPORT_MODPORT_MODID_GET      get a modid value from a modport gport
 * GPORT_MODPORT_PORT_GET       get a port value from a modport gport
 * GPORT_TRUNK_SET              set a gport trunk type and value
 * GPORT_TRUNK_GET              get a trunk_id value from a trunk gport
 * GPORT_VLAN_PORT_ID_SET       set a VLAN port ID type and value
 * GPORT_VLAN_PORT_ID_GET       get a VLAN port ID from a VLAN gport
 */

#ifndef _SHR_GPORT_H
#define _SHR_GPORT_H
/*
 * Typedef:
 *    _shr_gport_t
 * Purpose:
 *    GPORT (generic port) type for shared definitions
 */
typedef uint32 _shr_gport_t;

/*
 * Defines:
 *     _SHR_GPORT_*
 * Purpose:
 *     GPORT (Generic port) definitions. GPORT can be used to specify
 *     a {module, port} pair, trunk, and other port types.
 */

#define _SHR_GPORT_NONE             (0)
#define _SHR_GPORT_INVALID         (-1)

#define _SHR_GPORT_TYPE_LOCAL               1  /* Port on local unit     */
#define _SHR_GPORT_TYPE_MODPORT             2  /* Module ID and port     */
#define _SHR_GPORT_TYPE_TRUNK               3  /* Trunk ID               */
#define _SHR_GPORT_TYPE_LOCAL_CPU           4  /* CPU destination        */
#define _SHR_GPORT_TYPE_MIRROR              5  /* Mirror destination.    */
#define _SHR_GPORT_TYPE_VLAN_PORT           6  /* VLAN port              */
#define _SHR_GPORT_TYPE_MCAST               7  /* Multicast Set          */
#define _SHR_GPORT_TYPE_MAX                _SHR_GPORT_TYPE_MCAST  /* used for sanity checks */


#define _SHR_GPORT_TYPE_SHIFT                           26
#define _SHR_GPORT_TYPE_MASK                            0x3f
#define _SHR_GPORT_MODID_SHIFT                          11
#define _SHR_GPORT_MODID_MASK                           0x7fff
#define _SHR_GPORT_PORT_SHIFT                           0
#define _SHR_GPORT_PORT_MASK                            0x7ff
#define _SHR_GPORT_TRUNK_SHIFT                          0
#define _SHR_GPORT_TRUNK_MASK                           0x3ffffff


#define _SHR_GPORT_MIRROR_SHIFT                         0
#define _SHR_GPORT_MIRROR_MASK                          0xffff
#define _SHR_GPORT_MIRROR_SUBTYPE_SHIFT 22
#define _SHR_GPORT_MIRROR_SUBTYPE_MASK 0xf
#define _SHR_GPORT_MIRROR_SUBTYPE_VAL_MIRROR 0
#define _SHR_GPORT_MIRROR_SUBTYPE_VAL_SNOOP 1
#define _SHR_GPORT_VLAN_PORT_SHIFT                      0
#define _SHR_GPORT_VLAN_PORT_MASK                       0x3ffffff
#define _SHR_GPORT_MCAST_SHIFT                          0
#define _SHR_GPORT_MCAST_MASK                           0x3ffffff



#define _SHR_GPORT_LOCAL_TYPE_COMMON (0)
#define _SHR_GPORT_LOCAL_TYPE_MASK (0x1f)
#define _SHR_GPORT_LOCAL_TYPE_SHIFT (21)


#define _SHR_GPORT_IS_SET(_gport)    \
        (((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) > 0) && \
         ((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) <= _SHR_GPORT_TYPE_MAX))

#define _SHR_GPORT_IS_LOCAL(_gport) \
        (((((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK) == \
          _SHR_GPORT_TYPE_LOCAL) && \
        ((((_gport) >> _SHR_GPORT_LOCAL_TYPE_SHIFT) & _SHR_GPORT_LOCAL_TYPE_MASK) == \
          _SHR_GPORT_LOCAL_TYPE_COMMON))

#define _SHR_GPORT_LOCAL_SET(_gport, _port)\
        ((_gport) = (_SHR_GPORT_TYPE_LOCAL << _SHR_GPORT_TYPE_SHIFT) |\
        (_SHR_GPORT_LOCAL_TYPE_COMMON << _SHR_GPORT_LOCAL_TYPE_SHIFT)|\
        (((_port) & _SHR_GPORT_PORT_MASK) << _SHR_GPORT_PORT_SHIFT))

#define _SHR_GPORT_LOCAL_GET(_gport) \
        (((_gport) >> _SHR_GPORT_PORT_SHIFT) & _SHR_GPORT_PORT_MASK)

#define _SHR_GPORT_IS_TRUNK(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_TRUNK)

#define _SHR_GPORT_TRUNK_SET(_gport, _trunk_id)                              \
        ((_gport) = (_SHR_GPORT_TYPE_TRUNK      << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_trunk_id) & _SHR_GPORT_TRUNK_MASK) << _SHR_GPORT_TRUNK_SHIFT))

#define _SHR_GPORT_TRUNK_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_TRUNK_SHIFT) & _SHR_GPORT_TRUNK_MASK)


#define _SHR_GPORT_LOCAL_CPU	    \
        (_SHR_GPORT_TYPE_LOCAL_CPU << _SHR_GPORT_TYPE_SHIFT)

#define _SHR_GPORT_IS_LOCAL_CPU(_gport)   ((_gport) == _SHR_GPORT_LOCAL_CPU)

#define _SHR_GPORT_IS_MIRROR(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MIRROR)

#define _SHR_GPORT_MIRROR_SET(_gport, _value)                               \
        ((_gport) = (_SHR_GPORT_TYPE_MIRROR << _SHR_GPORT_TYPE_SHIFT)   | \
         (((_value) & _SHR_GPORT_MIRROR_MASK) << _SHR_GPORT_MIRROR_SHIFT))

#define _SHR_GPORT_MIRROR_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_MIRROR_SHIFT) & _SHR_GPORT_MIRROR_MASK)


#define _SHR_GPORT_IS_VLAN_PORT(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_VLAN_PORT)

#define _SHR_GPORT_VLAN_PORT_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_VLAN_PORT << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_VLAN_PORT_MASK) << _SHR_GPORT_VLAN_PORT_SHIFT))

#define _SHR_GPORT_VLAN_PORT_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_VLAN_PORT_SHIFT) & _SHR_GPORT_VLAN_PORT_MASK)

#define _SHR_GPORT_IS_MCAST(_gport) \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) == _SHR_GPORT_TYPE_MCAST)

#define _SHR_GPORT_MCAST_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_MCAST_MASK) << _SHR_GPORT_MCAST_SHIFT))

#define _SHR_GPORT_MCAST_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MCAST_SHIFT) & _SHR_GPORT_MCAST_MASK)


#endif  /* !_SHR_GPORT_H */

