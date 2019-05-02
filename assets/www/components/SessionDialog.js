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
      <v-flex xs12 sm6 md3>
        <span>List of sessions {{currentSessionClone}}</span>
        <v-select
            :items="sessions"
            v-model='currentSessionClone'
            label="List of sessions"
            persistent-hint
            return-object
            single-line
          ></v-select>
    </v-flex>
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
        currentSessionClone: this.currentSession
      }
  },
  computed: {
    show: {
      get () {
        return this.visible
      },
      set (value) {
        if (!value) {
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
                value: iso,
                text: iso
            }
            items.push(entry);
        });

        return items;
    }
  },
  methods: {

  }
}


