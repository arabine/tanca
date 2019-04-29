
var results_view_template = /*template*/`

<v-layout column>
  <v-flex xs12 >
      
    <v-data-table :headers="headers"
                :items="items"
                expand
                class="elevation-1">
      <template slot="items" slot-scope="props">
        <tr>
          <td class="text-xs">{{ props.item.rank }}</td>
          <td class="text-xs">{{ props.item.team }}</td>
          <td class="text-xs">{{ props.item.total_wins }}</td>
          <td class="text-xs">{{ props.item.total_losses }}</td>
          <td class="text-xs">{{ props.item.diff }}</td>
          <td class="text-xs">{{ props.item.sos }}</td>
        </tr>
      </template>
    </v-data-table>

    </v-flex>
</v-layout>
`;

ResultsView = {
  name: 'results-view',
  template: results_view_template,
  //====================================================================================================================
  components: {
    
  },
  data () {
    return {
      headers: [
        { text: 'Rank', value: 'rank' },
        { text: 'Team', value: 'team' },
        { text: 'Wins', value: 'total_wins' },
        { text: 'Losses', value: 'total_losses' },
        { text: 'Diff', value: 'diff' },
        { text: 'SPA', value: 'sos' },
      ],
      items: [

      ]
    }
  },
  computed: {
     
      
  },
  //====================================================================================================================
  created() {
    this.$store.dispatch('updateRanking').then( (ranking) => {
      console.log("[RESULTS] Update ranking ok");
      for (let i = 0; i < ranking.length; i++) {
        ranking[i].rank = (i+1);
        let t = this.$store.getters.getTeamById(ranking[i].id);
        ranking[i].team = '(' + ranking[i].id + ') ' + this.$store.getters.getTeamName(t);
      }
      this.items = ranking;

    }).catch( (err) => {
      this.$eventHub.$emit('alert', "Impossible de créer le classement: " + err, 'error');
    });
  },
  //====================================================================================================================
  beforeDestroy() {
 
  },
  //====================================================================================================================
  methods : {
   
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component ResultsView');
  },
}
