#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

class Protocol
{
 public:
   Protocol();
   ~Protocol();
   void parseCommand();
   void sendCommand();
};

/* A list of valid MCP commands. Almost every command can be sent from server
 * to client, but only some commands can be sent from client to server. These
 * commands are marked with a #.
 */

/* 0x00?? - Connection control */

#define CMD_START        0x0001  /* Activate client             */
#define CMD_ASK_NAME     0x0011  /* Ask for name                */
#define CMD_LOGIN_NAME   0x0012  /* (#) Player name             */
#define CMD_ASK_PASS     0x0013  /* Ask for password            */
#define CMD_LOGIN_PASS   0x0014  /* (#) Player password         */

#define CMD_REBOOT       0x00F0  /* Server is rebooting         */
#define CMD_SHUTDOWN     0x00F1  /* Server is shutting down     */
#define CMD_CRASH        0x00F2  /* Server is crashing!         */
#define CMD_QUIT         0x00FF  /* Server tells client to quit */

/* 0x01?? - Messages to player */

#define CMD_SEE          0x0101  /* Player sees something       */
#define CMD_HEAR         0x0102  /* Player hears something      */
#define CMD_SMELL        0x0103  /* Player smells something     */
#define CMD_FEEL         0x0104  /* Player feels (touches) sth  */
#define CMD_TASTE        0x0105  /* Player tastes something     */
#define CMD_SENSE        0x0106  /* Player senses something     */

#define CMD_OOC          0x0180  /* Out-of-character message    */
#define CMD_INFO         0x0181  /* Info: We will reboot soon   */ 

#define CMD_MSG          0x01FF  /* Message that is always seen */

/* 0x02?? - Client setup */

#define CMD_VIS          0x0201  /* Set player visibility       */
#define CMD_LTMIN        0x0202  /* Set minimum light required  */
#define CMD_LTMAX        0x0203  /* Set maximum light required  */
#define CMD_XYPOS        0x0204  /* Position player on map      */
#define CMD_MAP          0x0210  /* Send the map to client      */
#define CMD_TILE         0x0211  /* Update tile in client's map */
#define CMD_SET_LIGHT    0x0212  /* Set the map's light value   */
#define CMD_DESCRIPTION  0x0213  /* Send description to client  */
#define CMD_SPEED        0x0214  /* Adjust delay loop in client */
#define CMD_FREEZE       0x0215  /* Freeze the player           */
#define CMD_LIST         0x0217  /* Messages to list window     */
#define CMD_PROMPT       0x0218  /* <command> <prompt>          */
#define CMD_SOUND        0x0219  /* Use soundfx in client       */
#define CMD_UPDATE_LIGHT 0x021A  /* Update client's lightmap    */
#define CMD_DIALOG       0x021B  /* Client side dialog API      */
#define CMD_TARGET       0x021C  /* Query for a target          */
#define CMD_ANIMATE      0x021D  /* Animated effects            */
   
#define CMD_ADD_OBJECT   0x0220  /* <oid>                       */
#define CMD_DEL_OBJECT   0x0221  /* <oid>                       */
#define CMD_INFO_OBJECT  0x0222  /* <oid> <object attributes>   */
#define CMD_MOVE_OBJECT  0x0223  /* <oid> <x> <y>               */
#define CMD_OID_OWN      0x0224
#define CMD_DEL_OBJECTS  0x022F  /* Clears the object list      */

#define CMD_LOCK_MAP     0x0230  /* Prevents map updating       */
#define CMD_RELEASE_MAP  0x0231  /* Allows map updating         */

#define CMD_MAP_SLICE    0x0240  /* Sends a slice of map        */ 
#define CMD_MAP_COORD    0x0241  /* Coords for upperleft corner */
   
/* 0x03?? - Player information */

#define CMD_PLR_HP       0x0301  /* Current hitpoints           */
#define CMD_PLR_SP       0x0302  /* Current spellpoints         */
#define CMD_PLR_EP       0x0303  /* Current endurance points    */

#define CMD_PLR_STAT     0x0310  /* Synchronize attributes      */

#define CMD_PLR_MAXHP    0x0341  /* Maximum hitpoints           */
#define CMD_PLR_MAXSP    0x0342  /* Maximum spellpoints         */
#define CMD_PLR_MAXEP    0x0343  /* Maximum endurance points    */

/* 0x04?? - Client-side calculations */

/* 0x10?? - Client -> server */

#define CMD_COMMAND      0x1001  /* Command typed by player     */
#define CMD_CHAR		 0x1002  /* Char pressed by player      */

#endif
