/**
 * WarfaceBot, a blind XMPP client for Warface (FPS)
 * Copyright (C) 2015, 2016 Levak Borok <levak92@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <wb_tools.h>
#include <wb_stream.h>
#include <wb_xmpp.h>
#include <wb_xmpp_wf.h>
#include <wb_session.h>
#include <wb_friend.h>

#include <stdlib.h>
#include <string.h>

static void _compute_own_position(const struct clanmate *c, void *args)
{
    if (session.profile.clan.points > c->clan_points)
    {
        unsigned int *p_own_position = (unsigned int *) args;

        --*p_own_position;
    }
}

static void xmpp_iq_peer_player_info_cb(const char *msg_id,
                                        const char *msg,
                                        void *args)
{
    /* Answer
       <iq from="xxx@warface/GameClient" id="xxxxxx" type="get">
        <query xmlns="urn:cryonline:k01">
         <peer_player_info/>
        </query>
       </iq>
    */

    char *jid = get_info(msg, "from='", "'", NULL);
    char *clan_stats;

    if (session.profile.clan.id != 0)
    {
        unsigned int own_position =
            session.profile.clanmates->length + 1;

        list_foreach(session.profile.clanmates,
                     (f_list_callback) _compute_own_position,
                     &own_position);

        FORMAT(clan_stats,
               "    clan_name='%s' clan_role='%i'"
               "    clan_position='%i' clan_points='%i'"
               "    clan_member_since='%X'",
               session.profile.clan.name,
               session.profile.clan.role,
               own_position,
               session.profile.clan.points,
               session.profile.clan.joined);
    }
    else
        clan_stats = strdup("");

    send_stream_format(session.wfs,
                       "<iq to='%s' id='%s' type='result'>"
                       " <query xmlns='urn:cryonline:k01'>"
                       "  <peer_player_info online_id='%s' nickname='%s'"
                       "    pvp_rating_points='%u'"
                       "    primary_weapon='%s'"
                       "    banner_badge='%u' banner_mark='%u'"
                       "    banner_stripe='%u' experience='%u'"
                       "    items_unlocked='%u'"
                       "    challenges_completed='%u'"
                       "    missions_completed='%u'"
                       "    pvp_wins='%u' pvp_loses='%u'"
                       "    pvp_kills='%u' pvp_deaths='%u'"
                       "    playtime_seconds='%u'"
                       "    leavings_percentage='%f'"
                       "    coop_climbs_performed='%u'"
                       "    coop_assists_performed='%u'"
                       "    favorite_pvp_class='%u' favorite_pve_class='%u'"
                       "    %s />"
                       " </query>"
                       "</iq>",
                       jid, msg_id,
                       session.xmpp.jid,
                       session.profile.nickname,
                       session.profile.stats.pvp.rating_points,
                       session.profile.primary_weapon,
                       session.profile.banner.badge,
                       session.profile.banner.mark,
                       session.profile.banner.stripe,
                       session.profile.experience,
                       session.profile.stats.items_unlocked,
                       session.profile.stats.challenges_completed,
                       session.profile.stats.pve.missions_completed,
                       session.profile.stats.pvp.wins,
                       session.profile.stats.pvp.loses,
                       session.profile.stats.pvp.kills,
                       session.profile.stats.pvp.deaths,
                       session.profile.stats.playtime_seconds,
                       session.profile.stats.leavings_percentage,
                       session.profile.stats.coop_climbs_performed,
                       session.profile.stats.coop_assists_performed,
                       session.profile.stats.pvp.favorite_class,
                       session.profile.stats.pve.favorite_class,
                       clan_stats);

    free(clan_stats);
    free(jid);
}

void xmpp_iq_peer_player_info_r(void)
{
    qh_register("peer_player_info", 1, xmpp_iq_peer_player_info_cb, NULL);
}
