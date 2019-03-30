var add_player_dialog_template = /*template*/`
<v-dialog v-model="show" fullscreen hide-overlay transition="dialog-bottom-transition" scrollable>
<v-card tile>
  <v-toolbar card dark color="primary">
    <v-btn icon dark @click.stop="show=false">
      <v-icon>close</v-icon>
    </v-btn>
    <v-toolbar-title>Add player</v-toolbar-title>
    <v-spacer></v-spacer>
    <v-toolbar-items>
      <v-btn dark flat @click.stop="savePlayer">Save</v-btn>
    </v-toolbar-items>
  </v-toolbar>

  <!-- CENTRE DU DIALOG --> 
  <v-card-text>
      <v-flex xs12 sm6 md3>
        <v-text-field v-model="firstname" :rules="[() => !!firstname || 'This field is required']" :error-messages="errorMessages" label="Firt name" required></v-text-field>
        <v-text-field v-model="lastname" :rules="[() => !!lastname || 'This field is required']" :error-messages="errorMessages" label="Last name" required></v-text-field>
    </v-flex>
  </v-card-text>
 
</v-card>
</v-dialog>`

AddPlayerDialog = {
  template: add_player_dialog_template,
  props: ['visible'],
  data() {
      return {
        errorMessages: '',
        formHasErrors: false,
        firstname: null,
        lastname: null,
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
    form () {
      return {
        firstname: this.firstname,
        lastname: this.lastname,
      }
    }
  },
  methods: {
    savePlayer: function() {

      this.formHasErrors = false

      Object.keys(this.form).forEach(f => {
        if (!this.form[f]) this.formHasErrors = true
      });

      if (!this.formHasErrors) {
        var player = {
          firstname: this.firstname,
          lastname: this.lastname
        }

        this.$store.dispatch('addPlayer', player).then( (doc) => {
          return console.log('[PLAYERS] Successfully added a player!');
        }).catch((err) => {
            console.log('[PLAYERS] Add player failure: ' + err);
            this.$eventHub.$emit('alert', "Erreur: impossible de créer le joueur", 'error');
        });
        this.$emit('close');
      }
    }


  }
}


