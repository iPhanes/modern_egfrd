#pragma once

// --------------------------------------------------------------------------------------------------------------------------------

#include "EventScheduler.hpp"
#include "World.hpp"
#include "ReactionRuleCollection.hpp"
#include "ReactionRecorder.hpp"

// --------------------------------------------------------------------------------------------------------------------------------

class ParticleSimulator
{
public:
   ParticleSimulator() = delete;

   explicit ParticleSimulator(World& world, ReactionRuleCollection& reaction_rules, RandomNumberGenerator& rng) noexcept
      : world_(world), reaction_rules_(reaction_rules), rng_(rng), time_(0.), dt_(0.), num_steps_(0), rrec_(nullptr) {}

   const World& world() const { return world_; }

   const ReactionRuleCollection& reaction_rules() const { return reaction_rules_; }

   RandomNumberGenerator& rng() const { return rng_; }

   double time() const { return time_; }

   double dt() const { return dt_; }

   size_t num_steps() const { return num_steps_; }

   void add_reaction_recorder(reaction_recorder* rrec) { rrec->link(rrec_); rrec_ = rrec; }     // chain in front of rrec_
   
   void remove_reaction_recorder(reaction_recorder* rrec)
   {
      if (!rrec_) return;     // no chain

      if (rrec_ == rrec)      // first chain?
      {
         rrec_ = rrec->next();
         rrec->link(nullptr);
         return;
      }

      auto p = rrec_;
      while (p->next())      // loop chain
      {
         if (p->next() == rrec) // when found, remove chain
         {
            p->link(rrec->next());
            rrec->link(nullptr);
            return;
         }
         p = p->next();
      }
   }

   void set_repulsive() const
   {
      // The EGFRDSimulator will assume: a repulsive reaction rule (k = 0) for each
      //   possible combination of reactants for which no reaction rule is specified.
      auto species = world_.get_species();
      for (auto& s1 : species)
         for (auto& s2 : species)
         {
            auto rr = reaction_rules_.query_reaction_rules(s1.first, s2.first);
            if (rr.size() == 0) reaction_rules_.add_reaction_rule(ReactionRule(ReactionRule::reactants(s1.first, s2.first), 0.0, {}));
         }
   }

protected:
   friend class Persistence;

   World& world_;
   ReactionRuleCollection& reaction_rules_;
   RandomNumberGenerator& rng_;
   double time_, dt_;
   size_t num_steps_;
   reaction_recorder* rrec_;
};

// --------------------------------------------------------------------------------------------------------------------------------
