#pragma once
#include <vector>

#include "ecs.hpp"
#include "components.hpp"
#include <typeinfo>

// adapted from A1 
class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<Player> players; 
	ComponentContainer<PlayerCurrentState> playerCurrentStates;
	ComponentContainer<Opponent> opponents; 
	ComponentContainer<Health> healths;
	ComponentContainer<Animation> animations;
	ComponentContainer<Motion> motions; 
    ComponentContainer<StationaryTimer> stationaryTimers; 
    ComponentContainer<PostureBar> postureBars; 
	ComponentContainer<StateTimer> stateTimers; 
	ComponentContainer<PlayerInput> playerInputs; 
	ComponentContainer<CollisionBox> collisionBoxes;
	ComponentContainer<HitBox> hitBoxes; 
	ComponentContainer<HurtBox> hurtBoxes; 
	ComponentContainer<ParryBox> parryBoxes; 
	ComponentContainer<PerfectParryBox> perfectParryBoxes; 
	ComponentContainer<Sprite> sprites; 
	ComponentContainer<Boundary> boundaries;
	ComponentContainer<PlayableArea> playableArea;
	ComponentContainer<BoundaryCollision> boundaryCollisions;

    // TODO: add render related components 
	ComponentContainer<Renderable> renderable;
	ComponentContainer<HitboxRender> debugRenders;
	ComponentContainer<StaticRender> staticRenders;

	ComponentContainer<ObjectMesh*> objectMeshPtrs;
	ComponentContainer<RenderRequest> renderRequests;

	ComponentContainer<KnockBack> knockbacks;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&players);
		registry_list.push_back(&playerCurrentStates);
        registry_list.push_back(&opponents);
        registry_list.push_back(&healths);
		registry_list.push_back(&animations);
        registry_list.push_back(&motions);
        registry_list.push_back(&stationaryTimers);
        registry_list.push_back(&postureBars);
        registry_list.push_back(&stateTimers);
        registry_list.push_back(&playerInputs);
		registry_list.push_back(&collisionBoxes);
        registry_list.push_back(&hitBoxes);
        registry_list.push_back(&hurtBoxes);
        registry_list.push_back(&parryBoxes);
        registry_list.push_back(&perfectParryBoxes);
        registry_list.push_back(&sprites);
		registry_list.push_back(&boundaries);
		registry_list.push_back(&playableArea);
		registry_list.push_back(&boundaryCollisions);
		registry_list.push_back(&renderable);
		registry_list.push_back(&debugRenders);
		registry_list.push_back(&staticRenders);

		registry_list.push_back(&objectMeshPtrs);
		registry_list.push_back(&renderRequests);

		registry_list.push_back(&knockbacks);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;