/* Swfdec
 * Copyright (C) 2007 Benjamin Otte <otte@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

/*** GENERAL ***/

ASSetNative = ASnative (4, 0);
ASSetNativeAccessor = ASnative (4, 1);

/*** ASBROADCASTER ***/

function AsBroadcaster () { };

AsBroadcaster.broadcastMessage = ASnative(101, 12);

AsBroadcaster.addListener = function (x) {
  this.removeListener (x);
  this._listeners.push (x);
  return true;
};

AsBroadcaster.removeListener = function (x) {
  var l = this._listeners;
  var i;

  for (var i = 0; i < l.length; i++) {
    if (l[i] == x) {
      l.splice (i, 1);
      return true;
    }
  }
  return false;
};

AsBroadcaster.initialize = function (o) {
  o.broadcastMessage = ASnative(101, 12);
  o.addListener = AsBroadcaster.addListener;
  o.removeListener = AsBroadcaster.removeListener;
  o._listeners = new Array ();
  ASSetPropFlags(o, "broadcastMessage,addListener,removeListener,_listeners", 131);
};
ASSetPropFlags(AsBroadcaster, null, 131);

Key = { ALT: 18, BACKSPACE: 8, CAPSLOCK: 20, CONTROL: 17, DELETEKEY: 46, 
    DOWN: 40, END: 35, ENTER: 13, ESCAPE: 27, HOME: 36, INSERT: 45, LEFT: 37, 
    PGDN: 34, PGUP: 33, RIGHT: 39, SHIFT: 16, SPACE: 32, TAB: 9, UP: 38 };
ASSetNative(Key, 800, "getAscii,getCode,isDown");
AsBroadcaster.initialize(Key);
ASSetPropFlags(Key, null, 7);

/*** MOUSE ***/

Mouse = new Object ();
Mouse.show = ASnative (5, 0);
Mouse.hide = ASnative (5, 1);
AsBroadcaster.initialize (Mouse);

/*** STAGE ***/

Stage = new Object ();
AsBroadcaster.initialize (Stage);
ASSetNativeAccessor (Stage, 666, "scaleMode,align,width,height", 1);

