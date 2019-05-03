var session_dialog_template = /*template*/`
<v-dialog v-model="show" fullscreen hide-overlay transition="dialog-bottom-transition" scrollable>
<v-card tile>
  <v-toolbar card dark color="primary">
    <v-btn icon dark @click.stop="show=false">
      <v-icon>close</v-icon>
    </v-btn>
    <v-toolbar-title>Sessions</v-toolbar-title>
  </v-toolbar>

  <!-- CENTRE DU DIALOG --> 
  <v-card-text>
    <v-layout column>
      <v-flex xs12 >
        <v-btn color="success" @click="newSession()">New session</v-btn>
        <v-data-table :headers="headers"
                :items="sessions"
                expand
                class="elevation-1">
          <template slot="items" slot-scope="props">
            <tr :style="{backgroundColor: props.item.isCurrent ? 'steelblue' : 'transparent' }">
              <td class="text-xs">{{ props.item.date }}</td>
              <td>
                <v-icon small @click="loadSession(props.item)">get_app</v-icon>
              </td>
              <td>
                <v-icon small @click="deleteSession(props.item)">delete</v-icon>
              </td>
            </tr>
          </template>
        </v-data-table>
         
    </v-flex>
    </v-layout>
  </v-card-text>
 
</v-card>
</v-dialog>`

SessionDialog = {
  template: session_dialog_template,
  props: {
    visible: {
        type: Boolean,
        default: false
    },
    currentSession: {
        type: String,
        default: ''
    }
  },
  data() {
      return {
        errorMessages: '',
        currentSessionClone: this.currentSession,
        headers: [
          { text: 'Session Date', value: 'date' },
          { text: 'Load', value: 'load' },
          { text: 'Delete', value: 'delete' },
        ]
      }
  },
  computed: {
    show: {
      get () {
        return this.visible
      },
      set (value) {
        if (!value) {
          this.$router.push('teams');
          this.$emit('close');
        }
      }
    },
    sessions () {
        let items = [];
        let sList = this.$store.getters.getSessions;
        sList.forEach(s => {

            let iso = s.replace('session:', '');
            let entry = {
                date_iso: iso,
                date: moment(iso).format("MMMM Do YYYY, H:mm"),
                isCurrent: iso == this.currentSessionClone
            }
            items.push(entry);
        });

        return items;
    }
  },
  methods: {
    newSession () {
      this.$store.dispatch('createSession').then( () => {
        this.$eventHub.$emit('alert', "Nouvelle session créée", 'success');
      }).catch( (err) => {
        this.$eventHub.$emit('alert', "Impossible de créer la session: " + err, 'error');
      });
    },
    deleteSession(item) {
      if (this.$store.getters.sessionExists(item.date_iso)) {
        this.$store.dispatch('deleteSession', 'session:' + item.date_iso).then( () => {
          this.$eventHub.$emit('alert', "Session supprimée", 'success');
        }).catch((err) => {
          this.$eventHub.$emit('alert', "Impossible de supprimer la session: " + err, 'error');
        });
      }
    },
    loadSession(item) {
      if (this.$store.getters.sessionExists(item.date_iso)) {
        this.$store.dispatch('loadSession', 'session:' + item.date_iso).then( () => {
          this.$eventHub.$emit('alert', "Session chargée", 'success');
        }).catch((err) => {
          this.$eventHub.$emit('alert', "Impossible de charger la session: " + err, 'error');
        });
      }
    }
  }
}


